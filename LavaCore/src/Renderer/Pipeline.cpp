#include "Lava/Lavapch.h"
#include "Lava/Renderer/Pipeline.h"

#include "Lava/Application.h"
#include "Lava/Resources.h"
#include "Lava/Components/BasicBody3D.h"
#include "Lava/Renderer/Initializers.h"
#include "Lava/Renderer/Vertex.h"
#include "Lava/Renderer/RenderObjects/Camera3D.h"

#include <glm/ext/matrix_clip_space.hpp>

using namespace Lava;

Pipeline::Pipeline()
	: m_context(
		Application::getInstance().getWindow().getContext())

{
	createVulkanDescriptorSetLayout();
	createVulkanGraphicsPipeline();

	createVulkanCommandPool();

	createShaderDataBuffers();
	createVulkanDescriptorPool();
	createVulkanDescriptorSets();
}

Pipeline::~Pipeline()
{
	vkDestroyDescriptorPool(m_context->getDevice(), m_descriptor_pool, nullptr);
	m_camera_uniform_buffers.clear();
	m_model_dynamic_uniform_buffers.clear();
	_aligned_free(m_model_data);

	vkDestroyPipeline(m_context->getDevice(), m_pipeline, nullptr);
	vkDestroyPipelineLayout(m_context->getDevice(), m_pipeline_layout, nullptr);
	vkDestroyShaderModule(m_context->getDevice(), m_vert_shader_module, nullptr);
	vkDestroyShaderModule(m_context->getDevice(), m_frag_shader_module, nullptr);
	vkDestroyDescriptorSetLayout(m_context->getDevice(), m_descriptor_set_layout, nullptr);
}

void Pipeline::draw(const VkCommandBuffer& command_buffer_, uint32_t current_frame_) const
{
	vkCmdBindPipeline(command_buffer_, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);

	VkViewport viewport;
	viewport.x        = 0.0f;
	viewport.y        = 0.0f;
	viewport.width    = static_cast<float>(m_context->getExtent2D().width);
	viewport.height   = static_cast<float>(m_context->getExtent2D().height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(command_buffer_, 0, 1, &viewport);

	VkRect2D scissor;
	scissor.offset = {0,0};
	scissor.extent = m_context->getExtent2D();
	vkCmdSetScissor(command_buffer_, 0, 1, &scissor);

	for (size_t i = 0; i < m_objects.size(); i++)
	{
		uint32_t dynamic_offset = static_cast<uint32_t>(i * m_model_dynamic_uniform_buffers[current_frame_].getAlignment());
		vkCmdBindDescriptorSets(command_buffer_, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline_layout, 0, 1, &m_descriptor_sets[current_frame_], 1, &dynamic_offset);
		m_objects[i]->drawIndexed(command_buffer_);
	}
}

void Pipeline::updateCameraUniformBuffer(uint32_t current_frame_)
{
	m_camera_data.view       = m_camera->getViewMatrix();
	m_camera_data.projection = glm::perspective(glm::radians(m_camera->getFov()), (float)m_context->getExtent2D().width / (float)m_context->getExtent2D().height, 0.1f, 100.0f);
	//m_camera_ubo_data.projection[1][1] *= -1;
	m_camera_data.calculateProjectionView();

	m_camera_uniform_buffers[current_frame_].updateMemory(&m_camera_data);
}

void Pipeline::updateModelDynamicUniformBuffer(uint32_t current_frame_)
{
	for (size_t i = 0; i < m_objects.size(); i++)
	{
		auto model       = reinterpret_cast<ModelData*>(reinterpret_cast<uintptr_t>(m_model_data) + (i * m_model_dynamic_uniform_buffers[0].getAlignment()));
		model->transform = m_objects[i]->transform_data->getTranslationMatrix();
		model->rotation  = m_objects[i]->transform_data->getRotationMatrix();
	}

	m_model_dynamic_uniform_buffers[current_frame_].updateMemory(m_model_data, static_cast<uint32_t>(m_objects.size()));
}

void Pipeline::pushObjects(const std::shared_ptr<Components::BasicBody3D>& object_)
{
	if (!object_->basic_body_3d.ready_to_draw && object_->basic_body_3d.ready_to_copy)
		object_->copyStgBuffersToGpu(m_copy_command_pool);

	m_objects.push_back(object_);
}

void Pipeline::pushObjects(const std::vector<std::shared_ptr<Components::BasicBody3D>>& objects_)
{
	for (auto& object : objects_)
	{
		if (!object->basic_body_3d.ready_to_draw && object->basic_body_3d.ready_to_copy)
			object->copyStgBuffersToGpu(m_copy_command_pool);

		m_objects.push_back(object);
	}
}

// //////////////////// //

void Pipeline::createVulkanDescriptorSetLayout()
{
	std::vector<VkDescriptorSetLayoutBinding> set_layout_bindings =
	{
		Initializers::descriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT),
		Initializers::descriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, VK_SHADER_STAGE_VERTEX_BIT)
	};

	VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info = Initializers::descriptorSetLayoutCreateInfo(set_layout_bindings);

	if (vkCreateDescriptorSetLayout(m_context->getDevice(), &descriptor_set_layout_create_info, nullptr, &m_descriptor_set_layout) != VK_SUCCESS)
		LAVA_CORE_ERROR("Failed to create descriptor set layout!");

	LAVA_CORE_DEBUG("Created: VkDescriptorSetLayout");
}

void Pipeline::createVulkanGraphicsPipeline()
{
	auto vert_shader_code = readShaderFile("basic.vert.spv");
	auto frag_shader_code = readShaderFile("basic.frag.spv");

	m_vert_shader_module = createShaderModule(vert_shader_code);
	m_frag_shader_module = createShaderModule(frag_shader_code);

	VkPipelineShaderStageCreateInfo shader_stage_create_info[] =
	{
		Initializers::pipelineShaderStageCreateInfo(VK_SHADER_STAGE_VERTEX_BIT, m_vert_shader_module),
		Initializers::pipelineShaderStageCreateInfo(VK_SHADER_STAGE_FRAGMENT_BIT, m_frag_shader_module)
	};

	auto binding_descriptions   = Vertex3Color::getBindingDescriptions();
	auto attribute_descriptions = Vertex3Color::getAttributeDescriptions();

	VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info    = Initializers::pipelineVertexInputStageCreateInfo(binding_descriptions, attribute_descriptions);
	VkPipelineInputAssemblyStateCreateInfo input_assembly_create_info      = Initializers::pipelineInputAssemblyStateCreateInfo();
	VkPipelineViewportStateCreateInfo viewport_state_create_info           = Initializers::pipelineViewportStateCreateInfo(1, 1);
	VkPipelineRasterizationStateCreateInfo rasterization_state_create_info = Initializers::pipelineRasterizationStateCreateInfo(VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE);
	VkPipelineMultisampleStateCreateInfo multisample_state_create_info     = Initializers::pipelineMultisampleStateCreateInfo(VK_SAMPLE_COUNT_1_BIT);

	std::vector<VkPipelineColorBlendAttachmentState> color_blend_attachment_states =
	{
		Initializers::pipelineColorBlendAttachmentState(VK_FALSE, VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT)
	};

	VkPipelineColorBlendStateCreateInfo color_blend_state_create_info = Initializers::pipelineColorBlendStateCreateInfo(color_blend_attachment_states);

	const std::vector<VkDynamicState> dynamic_states = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};

	VkPipelineDynamicStateCreateInfo dynamic_state_create_info = Initializers::pipelineDynamicStateCreateInfo(dynamic_states);

	VkPipelineLayoutCreateInfo pipeline_layout_create_info = Initializers::pipelineLayoutCreateInfo(m_descriptor_set_layout);

	if (vkCreatePipelineLayout(m_context->getDevice(), &pipeline_layout_create_info, nullptr, &m_pipeline_layout) != VK_SUCCESS)
		LAVA_CORE_ERROR("Failed to create pipeline layout!");

	LAVA_CORE_DEBUG("Created: VkPipelineLayout");

	VkGraphicsPipelineCreateInfo pipeline_create_info;
	pipeline_create_info.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipeline_create_info.pNext               = nullptr;
	pipeline_create_info.flags               = NULL;
	pipeline_create_info.stageCount          = 2;
	pipeline_create_info.pStages             = shader_stage_create_info;
	pipeline_create_info.pVertexInputState   = &vertex_input_state_create_info;
	pipeline_create_info.pInputAssemblyState = &input_assembly_create_info;
	pipeline_create_info.pTessellationState  = nullptr;
	pipeline_create_info.pViewportState      = &viewport_state_create_info;
	pipeline_create_info.pRasterizationState = &rasterization_state_create_info;
	pipeline_create_info.pMultisampleState   = &multisample_state_create_info;
	pipeline_create_info.pDepthStencilState  = nullptr;
	pipeline_create_info.pColorBlendState    = &color_blend_state_create_info;
	pipeline_create_info.pDynamicState       = &dynamic_state_create_info;
	pipeline_create_info.layout              = m_pipeline_layout;
	pipeline_create_info.renderPass          = m_context->getRenderPass();
	pipeline_create_info.subpass             = 0;
	pipeline_create_info.basePipelineHandle  = VK_NULL_HANDLE;
	pipeline_create_info.basePipelineIndex   = -1;

	if (vkCreateGraphicsPipelines(m_context->getDevice(), VK_NULL_HANDLE, 1, &pipeline_create_info, nullptr, &m_pipeline) != VK_SUCCESS)
		LAVA_CORE_ERROR("Failed to create graphics pipeline!");
	LAVA_CORE_DEBUG("Created: VkPipeline");
}

void Pipeline::createVulkanCommandPool()
{
	VkCommandPoolCreateInfo command_pool_create_info = Initializers::commandPoolCreateInfo(VK_COMMAND_POOL_CREATE_TRANSIENT_BIT, m_context->getGraphicsQueueIndex());

	if (vkCreateCommandPool(m_context->getDevice(), &command_pool_create_info, nullptr, &m_copy_command_pool) != VK_SUCCESS)
		LAVA_CORE_ERROR("Failed to create command pool!");
	LAVA_CORE_DEBUG("Created: VkCommandPool");
}

//

void Pipeline::createShaderDataBuffers()
{
	// camera
	m_camera_uniform_buffers.resize(m_context->getFramesCount());

	// model
	m_model_dynamic_uniform_buffers.reserve(m_context->getFramesCount());
	for (size_t i = 0; i < m_context->getFramesCount(); i++)
		m_model_dynamic_uniform_buffers.emplace_back(128);

	m_model_data = static_cast<ModelData*>(_aligned_malloc(128, m_model_dynamic_uniform_buffers[0].getAlignment()));
}

void Pipeline::createVulkanDescriptorPool()
{
	std::vector<VkDescriptorPoolSize> pool_sizes =
	{
		Initializers::descriptorPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, m_context->getFramesCount()),
		Initializers::descriptorPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, m_context->getFramesCount())
	};

	VkDescriptorPoolCreateInfo pool_create_info = Initializers::descriptorPoolCreateInfo(pool_sizes, m_context->getFramesCount());

	if (vkCreateDescriptorPool(m_context->getDevice(), &pool_create_info, nullptr, &m_descriptor_pool) != VK_SUCCESS)
		LAVA_CORE_ERROR("Failed to create VkDescriptorPool!");
}

void Pipeline::createVulkanDescriptorSets()
{
	std::vector<VkDescriptorSetLayout> layouts(m_context->getFramesCount(), m_descriptor_set_layout);
	VkDescriptorSetAllocateInfo descriptor_set_allocate_info = Initializers::descriptorSetAllocateInfo(m_descriptor_pool, layouts);

	m_descriptor_sets.resize(m_context->getFramesCount());
	if (vkAllocateDescriptorSets(m_context->getDevice(), &descriptor_set_allocate_info, m_descriptor_sets.data()) != VK_SUCCESS)
		LAVA_CORE_ERROR("Failed to allocate descriptor sets!");

	for (size_t i = 0; i < m_context->getFramesCount(); i++)
	{
		VkDescriptorBufferInfo static_buffer_info        = m_camera_uniform_buffers[i].getDescriptorBufferInfo();
		VkWriteDescriptorSet static_write_descriptor_set = Initializers::writeDescriptorSet(m_descriptor_sets[i], 0, 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, static_buffer_info);
		vkUpdateDescriptorSets(m_context->getDevice(), 1, &static_write_descriptor_set, 0, nullptr);

		VkDescriptorBufferInfo dynamic_buffer_info        = m_model_dynamic_uniform_buffers[i].getDescriptorBufferInfo();
		VkWriteDescriptorSet dynamic_write_descriptor_set = Initializers::writeDescriptorSet(m_descriptor_sets[i], 1, 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, dynamic_buffer_info);
		vkUpdateDescriptorSets(m_context->getDevice(), 1, &dynamic_write_descriptor_set, 0, nullptr);
	}
}

//

std::vector<char> Pipeline::readShaderFile(const std::string& filename_) const
{
	std::filesystem::path shaders_path = Resources::getDir(ResourceDir::Shaders);
	std::ifstream file(shaders_path /= filename_, std::ios::ate | std::ios::binary);

	if (!file.is_open())
		LAVA_CORE_ERROR("Failed to open shader file!");

	const std::streamsize file_size = file.tellg();
	std::vector<char> buffer(file_size);
	file.seekg(0);
	file.read(buffer.data(), file_size);
	file.close();

	return buffer;
}

VkShaderModule Pipeline::createShaderModule(const std::vector<char>& code_) const
{
	VkShaderModuleCreateInfo shader_module_create_info;
	shader_module_create_info.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	shader_module_create_info.pNext    = nullptr;
	shader_module_create_info.flags    = NULL;
	shader_module_create_info.codeSize = code_.size();
	shader_module_create_info.pCode    = reinterpret_cast<const uint32_t*>(code_.data());

	VkShaderModule shader_module;
	if (vkCreateShaderModule(m_context->getDevice(), &shader_module_create_info, nullptr, &shader_module) != VK_SUCCESS)
		LAVA_CORE_ERROR("Failed to create shader module!");

	return shader_module;
}
