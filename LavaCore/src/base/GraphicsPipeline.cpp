#include "GraphicsPipeline.hpp"

#include "RenderPass.hpp"

#include <vulkan/vulkan.h>

#include <fstream>
#include <iostream>

using namespace lvc;

GraphicsPipeline::GraphicsPipeline(const VkDevice& t_device,
																	 const VkExtent2D& t_swapchain_extent,
																	 const VkRenderPass& t_render_pass,
																	 RenderPass& t_rpass)
	: m_device(t_device)
	, m_swapchain_extent(t_swapchain_extent)
	, m_render_pass(t_render_pass)
{
	const auto vert_shader_code = readShaderFile("shaders/basic.vert.spv");
	const auto frag_shader_code = readShaderFile("shaders/basic.frag.spv");

	m_vert_shader_module = createShaderModule(vert_shader_code);
	m_frag_shader_module = createShaderModule(frag_shader_code);

	VkPipelineShaderStageCreateInfo vert_shader_stage_create_info{};
	vert_shader_stage_create_info.sType               = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vert_shader_stage_create_info.pNext               = nullptr;
	vert_shader_stage_create_info.flags               = 0;
	vert_shader_stage_create_info.stage               = VK_SHADER_STAGE_VERTEX_BIT;
	vert_shader_stage_create_info.module              = m_vert_shader_module;
	vert_shader_stage_create_info.pName               = "main";
	vert_shader_stage_create_info.pSpecializationInfo = nullptr;

	VkPipelineShaderStageCreateInfo frag_shader_stage_create_info{};
	frag_shader_stage_create_info.sType               = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	frag_shader_stage_create_info.pNext               = nullptr;
	frag_shader_stage_create_info.flags               = 0;
	frag_shader_stage_create_info.stage               = VK_SHADER_STAGE_FRAGMENT_BIT;
	frag_shader_stage_create_info.module              = m_frag_shader_module;
	frag_shader_stage_create_info.pName               = "main";
	frag_shader_stage_create_info.pSpecializationInfo = nullptr;

	VkPipelineShaderStageCreateInfo shader_stage_create_info[] = {vert_shader_stage_create_info,
																																frag_shader_stage_create_info};

	VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info;
	vertex_input_state_create_info.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertex_input_state_create_info.vertexBindingDescriptionCount   = 0;
	vertex_input_state_create_info.pVertexBindingDescriptions      = nullptr;
	vertex_input_state_create_info.vertexAttributeDescriptionCount = 0;
	vertex_input_state_create_info.pVertexAttributeDescriptions    = nullptr;
	vertex_input_state_create_info.flags                           = 0;
	vertex_input_state_create_info.pNext                           = VK_NULL_HANDLE;

	VkPipelineInputAssemblyStateCreateInfo input_assembly_create_info{};
	input_assembly_create_info.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	input_assembly_create_info.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	input_assembly_create_info.primitiveRestartEnable = VK_FALSE;

	const std::vector<VkDynamicState> dynamic_states = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};

	VkPipelineDynamicStateCreateInfo dynamic_state_create_info{};
	dynamic_state_create_info.sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamic_state_create_info.dynamicStateCount = static_cast<uint32_t>(dynamic_states.size());
	dynamic_state_create_info.pDynamicStates    = dynamic_states.data();

	VkPipelineViewportStateCreateInfo viewport_state_create_info{};
	viewport_state_create_info.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewport_state_create_info.viewportCount = 1;
	viewport_state_create_info.scissorCount  = 1;

	VkPipelineRasterizationStateCreateInfo rasterization_state_create_info{};
	rasterization_state_create_info.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterization_state_create_info.depthClampEnable        = VK_FALSE;
	rasterization_state_create_info.rasterizerDiscardEnable = VK_FALSE;
	rasterization_state_create_info.polygonMode             = VK_POLYGON_MODE_FILL;
	rasterization_state_create_info.lineWidth               = 1.0f;
	rasterization_state_create_info.cullMode                = VK_CULL_MODE_BACK_BIT;
	rasterization_state_create_info.frontFace               = VK_FRONT_FACE_CLOCKWISE;
	rasterization_state_create_info.depthBiasEnable         = VK_FALSE;
	rasterization_state_create_info.depthBiasConstantFactor = 0.0f;
	rasterization_state_create_info.depthBiasClamp          = 0.0f;
	rasterization_state_create_info.depthBiasSlopeFactor    = 0.0f;

	VkPipelineMultisampleStateCreateInfo multisample_state_create_info{};
	multisample_state_create_info.sType                 = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisample_state_create_info.sampleShadingEnable   = VK_FALSE;
	multisample_state_create_info.rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT;
	multisample_state_create_info.minSampleShading      = 1.0f;
	multisample_state_create_info.pSampleMask           = nullptr;
	multisample_state_create_info.alphaToCoverageEnable = VK_FALSE;
	multisample_state_create_info.alphaToOneEnable      = VK_FALSE;

	VkPipelineColorBlendAttachmentState color_blend_attachment_state{};
	color_blend_attachment_state.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
																								VK_COLOR_COMPONENT_G_BIT |
																								VK_COLOR_COMPONENT_B_BIT |
																								VK_COLOR_COMPONENT_A_BIT;
	color_blend_attachment_state.blendEnable         = VK_FALSE;
	color_blend_attachment_state.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
	color_blend_attachment_state.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
	color_blend_attachment_state.colorBlendOp        = VK_BLEND_OP_ADD;
	color_blend_attachment_state.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	color_blend_attachment_state.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	color_blend_attachment_state.alphaBlendOp        = VK_BLEND_OP_ADD;

	VkPipelineColorBlendStateCreateInfo color_blend_state_create_info{};
	color_blend_state_create_info.sType             = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	color_blend_state_create_info.logicOpEnable     = VK_FALSE;
	color_blend_state_create_info.logicOp           = VK_LOGIC_OP_COPY;
	color_blend_state_create_info.attachmentCount   = 1;
	color_blend_state_create_info.pAttachments      = &color_blend_attachment_state;
	color_blend_state_create_info.blendConstants[0] = 0.0f;
	color_blend_state_create_info.blendConstants[1] = 0.0f;
	color_blend_state_create_info.blendConstants[2] = 0.0f;
	color_blend_state_create_info.blendConstants[3] = 0.0f;

	VkPipelineLayoutCreateInfo pipeline_layout_create_info{};
	pipeline_layout_create_info.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipeline_layout_create_info.setLayoutCount         = 0;
	pipeline_layout_create_info.pSetLayouts            = nullptr;
	pipeline_layout_create_info.pushConstantRangeCount = 0;
	pipeline_layout_create_info.pPushConstantRanges    = nullptr;

	if (vkCreatePipelineLayout(m_device, &pipeline_layout_create_info, nullptr, &m_pipeline_layout) != VK_SUCCESS)
		throw std::runtime_error("err: Failed to create pipeline layout!\n");

	VkGraphicsPipelineCreateInfo pipeline_create_info{};
	pipeline_create_info.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipeline_create_info.stageCount          = 2;
	pipeline_create_info.pStages             = shader_stage_create_info;
	pipeline_create_info.pVertexInputState   = &vertex_input_state_create_info;
	pipeline_create_info.pInputAssemblyState = &input_assembly_create_info;
	pipeline_create_info.pViewportState      = &viewport_state_create_info;
	pipeline_create_info.pRasterizationState = &rasterization_state_create_info;
	pipeline_create_info.pMultisampleState   = &multisample_state_create_info;
	pipeline_create_info.pDepthStencilState  = nullptr;
	pipeline_create_info.pColorBlendState    = &color_blend_state_create_info;
	pipeline_create_info.pDynamicState       = &dynamic_state_create_info;
	pipeline_create_info.layout              = m_pipeline_layout;
	pipeline_create_info.renderPass          = m_render_pass;
	pipeline_create_info.subpass             = 0;
	pipeline_create_info.basePipelineHandle  = VK_NULL_HANDLE;
	pipeline_create_info.basePipelineIndex   = -1;

	if (vkCreateGraphicsPipelines(m_device,VK_NULL_HANDLE, 1, &pipeline_create_info, nullptr, &m_pipeline) != VK_SUCCESS)
		throw std::runtime_error("err: Failed to create graphics pipeline!\n");

	t_rpass.createFrameBuffers();
}

GraphicsPipeline::~GraphicsPipeline()
{
	vkDestroyPipeline(m_device, m_pipeline, nullptr);
	std::clog << "Successfully destroyed graphics pipeline\n";
	vkDestroyPipelineLayout(m_device, m_pipeline_layout, nullptr);
	std::clog << "Successfully destroyed pipeline layout\n";
	vkDestroyShaderModule(m_device, m_vert_shader_module, nullptr);
	vkDestroyShaderModule(m_device, m_frag_shader_module, nullptr);
	std::clog << "Successfully destroyed shader modules\n";
}

VkShaderModule GraphicsPipeline::createShaderModule(const std::vector<char>& code) const
{
	VkShaderModuleCreateInfo create_info{};
	create_info.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	create_info.codeSize = code.size();
	create_info.pCode    = reinterpret_cast<const uint32_t*>(code.data());

	VkShaderModule shader_module;
	if (vkCreateShaderModule(m_device, &create_info, nullptr, &shader_module) != VK_SUCCESS)
		throw std::runtime_error("err: Failed to create shader module!\n");

	return shader_module;
}

std::vector<char> GraphicsPipeline::readShaderFile(const std::string& filename)
{
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open())
		throw std::runtime_error("err: Failed to open shader file!\n");

	const std::streamsize file_size = file.tellg();
	std::vector<char> buffer(file_size);
	file.seekg(0);
	file.read(buffer.data(), file_size);
	file.close();

	return buffer;
}
