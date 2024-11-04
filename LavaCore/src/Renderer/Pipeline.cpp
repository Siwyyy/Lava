#include "Lava/Lavapch.h"
#include "Lava/Renderer/Pipeline.h"

#include "Lava/Application.h"
#include "Lava/Resources.h"
#include "Lava/Input/Input.h"
#include "Lava/Input/KeyCodes.h"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "Lava/Renderer/BasicBody3DModel.h"

using namespace Lava;

Pipeline::Pipeline()
	: m_context(
		Application::getInstance().getWindow().getContext())

{
	createVulkanDescriptorSetLayout();
	createVulkanGraphicsPipeline();

	createVulkanCommandPool();

	createVulkanUniformBuffers();
	createVulkanDescriptorPool();
	createVulkanDescriptorSets();
}

Pipeline::~Pipeline()
{
	vkDestroyDescriptorPool(m_context->getDevice(), m_descriptor_pool, nullptr);
	for (size_t i = 0; i < m_context->getFramesInFlight(); i++)
	{
		vkDestroyBuffer(m_context->getDevice(), m_uniform_buffers[i], nullptr);
		vkFreeMemory(m_context->getDevice(), m_uniform_buffers_memory[i], nullptr);
	}

	vkDestroyPipeline(m_context->getDevice(), m_pipeline, nullptr);
	vkDestroyPipelineLayout(m_context->getDevice(), m_pipeline_layout, nullptr);
	vkDestroyShaderModule(m_context->getDevice(), m_vert_shader_module, nullptr);
	vkDestroyShaderModule(m_context->getDevice(), m_frag_shader_module, nullptr);
	vkDestroyDescriptorSetLayout(m_context->getDevice(), m_descriptor_set_layout, nullptr);
}

void Pipeline::draw(const VkCommandBuffer& command_buffer_, uint32_t current_frame_)
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

	vkCmdBindDescriptorSets(command_buffer_, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline_layout, 0, 1, &m_descriptor_sets[current_frame_], 0, nullptr);

	for (auto object : m_objects)
	{
		object->drawIndexed(command_buffer_);
	}
	//
	//
	//
	//
	//

	//const VkBuffer vertex_buffers2[] = {m_vertex_buffer2};
	//const VkDeviceSize offsets2[]    = {0};
	//vkCmdBindVertexBuffers(command_buffer_, 0, 1, vertex_buffers2, offsets2);
	//vkCmdBindIndexBuffer(command_buffer_, m_index_buffer2, 0, VK_INDEX_TYPE_UINT32);
	//vkCmdDrawIndexed(command_buffer_, static_cast<uint32_t>(INDICES2.size()), 1, 0, 0, 0);
}

void Pipeline::updateVulkanUniformBuffer(uint32_t current_frame_)
{
	static auto start_time = std::chrono::high_resolution_clock::now();
	static auto rotation   = glm::mat4(1.0f);

	std::chrono::time_point<std::chrono::steady_clock> static last_time;
	auto current_time = std::chrono::high_resolution_clock::now();
	if ((float)((current_time - start_time).count()) < 0.01f)
		last_time = current_time;
	float time = std::chrono::duration<float>(current_time - last_time).count();

	static auto x = .0f, y = 2.0f, z = 2.0f;
	if (Input::isKeyPressed(LAVA_KEY_W) && y > -10.f) { y -= .001f; }
	if (Input::isKeyPressed(LAVA_KEY_S) && y < 10.f) { y += .001f; }
	if (Input::isKeyPressed(LAVA_KEY_A) && x > -10.f) { x += .001f; }
	if (Input::isKeyPressed(LAVA_KEY_D) && x < 10.f) { x -= .001f; }
	if (Input::isKeyPressed(LAVA_KEY_SPACE) && z < 10.f) { z += .001f; }
	if (Input::isKeyPressed(LAVA_KEY_LEFT_SHIFT) && z > 1.f) { z -= .001f; }

	UniformBufferObject ubo;
	ubo.model = glm::rotate(rotation, time * glm::radians(90.f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.view  = glm::lookAt(glm::vec3(x, y, z), glm::vec3(x, y - 2.f, z - 2.f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.proj  = glm::perspective(glm::radians(45.0f), (float)m_context->getExtent2D().width / (float)m_context->getExtent2D().height, 0.1f, 100.0f);

	ubo.proj[1][1] *= -1;

	memcpy(m_uniform_buffers_mapped[current_frame_], &ubo, sizeof(ubo));
	last_time = current_time;
	rotation  = ubo.model;
}

void Pipeline::pushObjects(const std::vector<std::shared_ptr<BasicBody3D>>& objects_)
{
	for (auto& object : objects_)
	{
		if (!object->state.ready_to_draw && object->state.ready_to_copy)
			object->copyStgBuffersToGpu(m_copy_command_pool);

		m_objects.push_back(object);
	}
}

// //////////////////// //

void Pipeline::createVulkanDescriptorSetLayout()
{
	VkDescriptorSetLayoutBinding ubo_layout_binding;
	ubo_layout_binding.binding            = 0;
	ubo_layout_binding.descriptorType     = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	ubo_layout_binding.descriptorCount    = 1;
	ubo_layout_binding.stageFlags         = VK_SHADER_STAGE_VERTEX_BIT;
	ubo_layout_binding.pImmutableSamplers = nullptr;

	VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info;
	descriptor_set_layout_create_info.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptor_set_layout_create_info.pNext        = nullptr;
	descriptor_set_layout_create_info.flags        = NULL;
	descriptor_set_layout_create_info.bindingCount = 1;
	descriptor_set_layout_create_info.pBindings    = &ubo_layout_binding;

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

	VkPipelineShaderStageCreateInfo vert_shader_stage_create_info;
	vert_shader_stage_create_info.sType               = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vert_shader_stage_create_info.pNext               = nullptr;
	vert_shader_stage_create_info.flags               = 0;
	vert_shader_stage_create_info.stage               = VK_SHADER_STAGE_VERTEX_BIT;
	vert_shader_stage_create_info.module              = m_vert_shader_module;
	vert_shader_stage_create_info.pName               = "main";
	vert_shader_stage_create_info.pSpecializationInfo = nullptr;

	VkPipelineShaderStageCreateInfo frag_shader_stage_create_info;
	frag_shader_stage_create_info.sType               = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	frag_shader_stage_create_info.pNext               = nullptr;
	frag_shader_stage_create_info.flags               = 0;
	frag_shader_stage_create_info.stage               = VK_SHADER_STAGE_FRAGMENT_BIT;
	frag_shader_stage_create_info.module              = m_frag_shader_module;
	frag_shader_stage_create_info.pName               = "main";
	frag_shader_stage_create_info.pSpecializationInfo = nullptr;

	VkPipelineShaderStageCreateInfo shader_stage_create_info[] = {vert_shader_stage_create_info,
																																frag_shader_stage_create_info};

	auto binding_description    = Vertex3Color::getBindingDescription();
	auto attribute_descriptions = Vertex3Color::getAttributeDescriptions();

	VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info;
	vertex_input_state_create_info.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertex_input_state_create_info.pNext                           = VK_NULL_HANDLE;
	vertex_input_state_create_info.flags                           = 0;
	vertex_input_state_create_info.vertexBindingDescriptionCount   = 1;
	vertex_input_state_create_info.pVertexBindingDescriptions      = &binding_description;
	vertex_input_state_create_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(attribute_descriptions.size());
	vertex_input_state_create_info.pVertexAttributeDescriptions    = attribute_descriptions.data();

	VkPipelineInputAssemblyStateCreateInfo input_assembly_create_info;
	input_assembly_create_info.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	input_assembly_create_info.pNext                  = nullptr;
	input_assembly_create_info.flags                  = NULL;
	input_assembly_create_info.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	input_assembly_create_info.primitiveRestartEnable = VK_FALSE;

	const std::vector<VkDynamicState> dynamic_states = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};

	VkPipelineDynamicStateCreateInfo dynamic_state_create_info;
	dynamic_state_create_info.sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamic_state_create_info.pNext             = nullptr;
	dynamic_state_create_info.flags             = NULL;
	dynamic_state_create_info.dynamicStateCount = static_cast<uint32_t>(dynamic_states.size());
	dynamic_state_create_info.pDynamicStates    = dynamic_states.data();

	VkPipelineViewportStateCreateInfo viewport_state_create_info;
	viewport_state_create_info.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewport_state_create_info.pNext         = nullptr;
	viewport_state_create_info.flags         = NULL;
	viewport_state_create_info.viewportCount = 1;
	viewport_state_create_info.pViewports    = nullptr; // nullptr because using dynamic state viewport
	viewport_state_create_info.scissorCount  = 1;
	viewport_state_create_info.pScissors     = nullptr; // nullptr because using dynamic state scissor

	VkPipelineRasterizationStateCreateInfo rasterization_state_create_info;
	rasterization_state_create_info.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterization_state_create_info.pNext                   = nullptr;
	rasterization_state_create_info.flags                   = NULL;
	rasterization_state_create_info.depthClampEnable        = VK_FALSE;
	rasterization_state_create_info.rasterizerDiscardEnable = VK_FALSE;
	rasterization_state_create_info.polygonMode             = VK_POLYGON_MODE_FILL;
	rasterization_state_create_info.cullMode                = VK_CULL_MODE_BACK_BIT;
	rasterization_state_create_info.frontFace               = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterization_state_create_info.depthBiasEnable         = VK_FALSE;
	rasterization_state_create_info.depthBiasConstantFactor = 0.0f;
	rasterization_state_create_info.depthBiasClamp          = 0.0f;
	rasterization_state_create_info.depthBiasSlopeFactor    = 0.0f;
	rasterization_state_create_info.lineWidth               = 1.0f;

	VkPipelineMultisampleStateCreateInfo multisample_state_create_info;
	multisample_state_create_info.sType                 = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisample_state_create_info.pNext                 = nullptr;
	multisample_state_create_info.flags                 = NULL;
	multisample_state_create_info.rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT;
	multisample_state_create_info.sampleShadingEnable   = VK_FALSE;
	multisample_state_create_info.minSampleShading      = 1.0f;
	multisample_state_create_info.pSampleMask           = nullptr;
	multisample_state_create_info.alphaToCoverageEnable = VK_FALSE;
	multisample_state_create_info.alphaToOneEnable      = VK_FALSE;

	VkPipelineColorBlendAttachmentState color_blend_attachment_state;
	color_blend_attachment_state.blendEnable         = VK_FALSE;
	color_blend_attachment_state.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
	color_blend_attachment_state.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
	color_blend_attachment_state.colorBlendOp        = VK_BLEND_OP_ADD;
	color_blend_attachment_state.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	color_blend_attachment_state.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	color_blend_attachment_state.alphaBlendOp        = VK_BLEND_OP_ADD;
	color_blend_attachment_state.colorWriteMask      = VK_COLOR_COMPONENT_R_BIT |
																										 VK_COLOR_COMPONENT_G_BIT |
																										 VK_COLOR_COMPONENT_B_BIT |
																										 VK_COLOR_COMPONENT_A_BIT;

	VkPipelineColorBlendStateCreateInfo color_blend_state_create_info;
	color_blend_state_create_info.sType             = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	color_blend_state_create_info.pNext             = nullptr;
	color_blend_state_create_info.flags             = NULL;
	color_blend_state_create_info.logicOpEnable     = VK_FALSE;
	color_blend_state_create_info.logicOp           = VK_LOGIC_OP_COPY;
	color_blend_state_create_info.attachmentCount   = 1;
	color_blend_state_create_info.pAttachments      = &color_blend_attachment_state;
	color_blend_state_create_info.blendConstants[0] = 0.0f;
	color_blend_state_create_info.blendConstants[1] = 0.0f;
	color_blend_state_create_info.blendConstants[2] = 0.0f;
	color_blend_state_create_info.blendConstants[3] = 0.0f;

	VkPipelineLayoutCreateInfo pipeline_layout_create_info;
	pipeline_layout_create_info.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipeline_layout_create_info.pNext                  = nullptr;
	pipeline_layout_create_info.flags                  = NULL;
	pipeline_layout_create_info.setLayoutCount         = 1;
	pipeline_layout_create_info.pSetLayouts            = &m_descriptor_set_layout;
	pipeline_layout_create_info.pushConstantRangeCount = 0;
	pipeline_layout_create_info.pPushConstantRanges    = nullptr;

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

//

void Pipeline::createVulkanCommandPool()
{
	VkCommandPoolCreateInfo command_pool_create_info;
	command_pool_create_info.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	command_pool_create_info.pNext            = nullptr;
	command_pool_create_info.flags            = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
	command_pool_create_info.queueFamilyIndex = m_context->getGraphicsQueueIndex();

	if (vkCreateCommandPool(m_context->getDevice(), &command_pool_create_info, nullptr, &m_copy_command_pool) != VK_SUCCESS)
		LAVA_CORE_ERROR("Failed to create command pool!");
	LAVA_CORE_DEBUG("Created: VkCommandPool");
}

//

void Pipeline::createVulkanVertexBuffer(const std::vector<Vertex3Color>& vertices_, VkBuffer& buffer_, VkDeviceMemory& memory_)
{
	VkDeviceSize buffer_size = sizeof(vertices_[0]) * vertices_.size();

	VkBuffer staging_buffer;
	VkDeviceMemory staging_buffer_memory;
	createVulkanBuffer(buffer_size,
										 VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
										 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
										 staging_buffer,
										 staging_buffer_memory);

	void* data;
	vkMapMemory(m_context->getDevice(), staging_buffer_memory, 0, buffer_size,NULL, &data);
	memcpy(data, vertices_.data(), (size_t)buffer_size);
	vkUnmapMemory(m_context->getDevice(), staging_buffer_memory);

	createVulkanBuffer(buffer_size,
										 VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
										 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
										 buffer_,
										 memory_);

	copyVulkanBuffer(staging_buffer, buffer_, buffer_size);
	vkDestroyBuffer(m_context->getDevice(), staging_buffer, nullptr);
	vkFreeMemory(m_context->getDevice(), staging_buffer_memory, nullptr);
}

void Pipeline::createVulkanIndexBuffer(const std::vector<uint32_t>& indices_, VkBuffer& buffer_, VkDeviceMemory& memory_)
{
	VkDeviceSize buffer_size = sizeof(indices_[0]) * indices_.size();

	VkBuffer staging_buffer;
	VkDeviceMemory staging_buffer_memory;
	createVulkanBuffer(buffer_size,
										 VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
										 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
										 staging_buffer,
										 staging_buffer_memory);

	void* data;
	vkMapMemory(m_context->getDevice(), staging_buffer_memory, 0, buffer_size, 0, &data);
	memcpy(data, indices_.data(), (size_t)buffer_size);
	vkUnmapMemory(m_context->getDevice(), staging_buffer_memory);

	createVulkanBuffer(buffer_size,
										 VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
										 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
										 buffer_,
										 memory_);

	copyVulkanBuffer(staging_buffer, buffer_, buffer_size);
	vkDestroyBuffer(m_context->getDevice(), staging_buffer, nullptr);
	vkFreeMemory(m_context->getDevice(), staging_buffer_memory, nullptr);
}

void Pipeline::createVulkanUniformBuffers()
{
	VkDeviceSize buffer_size = sizeof(UniformBufferObject);

	m_uniform_buffers.resize(m_context->getFramesInFlight());
	m_uniform_buffers_memory.resize(m_context->getFramesInFlight());
	m_uniform_buffers_mapped.resize(m_context->getFramesInFlight());

	for (size_t i = 0; i < m_context->getFramesInFlight(); i++)
	{
		createVulkanBuffer(buffer_size,
											 VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
											 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
											 m_uniform_buffers[i],
											 m_uniform_buffers_memory[i]);

		vkMapMemory(m_context->getDevice(), m_uniform_buffers_memory[i], 0, buffer_size,NULL, &m_uniform_buffers_mapped[i]);
	}
}

void Pipeline::createVulkanDescriptorPool()
{
	VkDescriptorPoolSize pool_size;
	pool_size.type            = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	pool_size.descriptorCount = m_context->getFramesInFlight();

	VkDescriptorPoolCreateInfo pool_create_info;
	pool_create_info.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	pool_create_info.pNext         = nullptr;
	pool_create_info.flags         = NULL;
	pool_create_info.maxSets       = m_context->getFramesInFlight();
	pool_create_info.poolSizeCount = 1;
	pool_create_info.pPoolSizes    = &pool_size;

	if (vkCreateDescriptorPool(m_context->getDevice(), &pool_create_info, nullptr, &m_descriptor_pool) != VK_SUCCESS)
		LAVA_CORE_ERROR("Failed to create VkDescriptorPool!");
}

void Pipeline::createVulkanDescriptorSets()
{
	std::vector<VkDescriptorSetLayout> layouts(m_context->getFramesInFlight(), m_descriptor_set_layout);
	VkDescriptorSetAllocateInfo set_allocate_info;
	set_allocate_info.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	set_allocate_info.pNext              = nullptr;
	set_allocate_info.descriptorPool     = m_descriptor_pool;
	set_allocate_info.descriptorSetCount = m_context->getFramesInFlight();
	set_allocate_info.pSetLayouts        = layouts.data();

	m_descriptor_sets.resize(m_context->getFramesInFlight());
	if (vkAllocateDescriptorSets(m_context->getDevice(), &set_allocate_info, m_descriptor_sets.data()) != VK_SUCCESS)
		LAVA_CORE_ERROR("Failed to allocate descriptor sets!");

	for (size_t i = 0; i < m_context->getFramesInFlight(); i++)
	{
		VkDescriptorBufferInfo buffer_info;
		buffer_info.buffer = m_uniform_buffers[i];
		buffer_info.offset = 0;
		buffer_info.range  = sizeof(UniformBufferObject);

		VkWriteDescriptorSet write_descriptor_set;
		write_descriptor_set.sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write_descriptor_set.pNext            = nullptr;
		write_descriptor_set.dstSet           = m_descriptor_sets[i];
		write_descriptor_set.dstBinding       = 0;
		write_descriptor_set.dstArrayElement  = 0;
		write_descriptor_set.descriptorCount  = 1;
		write_descriptor_set.descriptorType   = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		write_descriptor_set.pImageInfo       = nullptr;
		write_descriptor_set.pBufferInfo      = &buffer_info;
		write_descriptor_set.pTexelBufferView = nullptr;

		vkUpdateDescriptorSets(m_context->getDevice(), 1, &write_descriptor_set, 0, nullptr);
	}
}

std::vector<char> Pipeline::readShaderFile(const std::string& filename_)
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

void Pipeline::createVulkanBuffer(VkDeviceSize size_, VkBufferUsageFlags usage_, VkMemoryPropertyFlags props_, VkBuffer& buffer_, VkDeviceMemory& buffer_memory_)
{
	VkBufferCreateInfo buffer_create_info;
	buffer_create_info.sType                 = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	buffer_create_info.pNext                 = nullptr;
	buffer_create_info.flags                 = NULL;
	buffer_create_info.size                  = size_;
	buffer_create_info.usage                 = usage_;
	buffer_create_info.sharingMode           = VK_SHARING_MODE_EXCLUSIVE;
	buffer_create_info.queueFamilyIndexCount = 0;
	buffer_create_info.pQueueFamilyIndices   = nullptr;

	if (vkCreateBuffer(m_context->getDevice(), &buffer_create_info, nullptr, &buffer_) != VK_SUCCESS)
		LAVA_CORE_ERROR("failed to create vertex buffer!");

	VkMemoryRequirements memory_requirements;
	vkGetBufferMemoryRequirements(m_context->getDevice(), buffer_, &memory_requirements);

	uint32_t memory_type_index = 0;
	bool found                 = false;
	VkPhysicalDeviceMemoryProperties properties;
	vkGetPhysicalDeviceMemoryProperties(m_context->getGpu(), &properties);
	for (uint32_t i = 0; i < properties.memoryTypeCount; i++)
		if ((memory_requirements.memoryTypeBits & (1 << i)) && (properties.memoryTypes[i].propertyFlags & props_) == props_)
		{
			memory_type_index = i;
			found             = true;
		}
	LAVA_ASSERT(found, "Failed to find memory type!")

	VkMemoryAllocateInfo allocate_info;
	allocate_info.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocate_info.pNext           = nullptr;
	allocate_info.allocationSize  = memory_requirements.size;
	allocate_info.memoryTypeIndex = memory_type_index;

	if (vkAllocateMemory(m_context->getDevice(), &allocate_info, nullptr, &buffer_memory_) != VK_SUCCESS)
		LAVA_CORE_ERROR("failed to allocate vertex buffer memory!");

	vkBindBufferMemory(m_context->getDevice(), buffer_, buffer_memory_, 0);
}

void Pipeline::copyVulkanBuffer(VkBuffer src_buffer_, VkBuffer dst_buffer_, VkDeviceSize size_)
{
	VkCommandBufferAllocateInfo command_buffer_allocate_info{};
	command_buffer_allocate_info.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	command_buffer_allocate_info.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	command_buffer_allocate_info.commandPool        = m_copy_command_pool;
	command_buffer_allocate_info.commandBufferCount = 1;

	VkCommandBuffer command_buffer;
	vkAllocateCommandBuffers(m_context->getDevice(), &command_buffer_allocate_info, &command_buffer);

	VkCommandBufferBeginInfo command_buffer_begin_info;
	command_buffer_begin_info.sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	command_buffer_begin_info.pNext            = nullptr;
	command_buffer_begin_info.flags            = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	command_buffer_begin_info.pInheritanceInfo = nullptr;

	vkBeginCommandBuffer(command_buffer, &command_buffer_begin_info);

	VkBufferCopy copy_region;
	copy_region.srcOffset = 0;
	copy_region.dstOffset = 0;
	copy_region.size      = size_;
	vkCmdCopyBuffer(command_buffer, src_buffer_, dst_buffer_, 1, &copy_region);

	vkEndCommandBuffer(command_buffer);

	VkSubmitInfo submit_info;
	submit_info.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.pNext                = nullptr;
	submit_info.waitSemaphoreCount   = 0;
	submit_info.pWaitSemaphores      = nullptr;
	submit_info.pWaitDstStageMask    = nullptr;
	submit_info.commandBufferCount   = 1;
	submit_info.pCommandBuffers      = &command_buffer;
	submit_info.signalSemaphoreCount = 0;
	submit_info.pSignalSemaphores    = nullptr;

	vkQueueSubmit(m_context->getGraphicsQueue(), 1, &submit_info,VK_NULL_HANDLE);
	vkQueueWaitIdle(m_context->getGraphicsQueue());

	vkFreeCommandBuffers(m_context->getDevice(), m_copy_command_pool, 1, &command_buffer);
}
