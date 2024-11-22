#pragma once

namespace Lava::Initializers
{
	inline VkDescriptorPoolSize descriptorPoolSize(VkDescriptorType type_,
																								 uint32_t count_)
	{
		VkDescriptorPoolSize descriptor_pool_size;
		descriptor_pool_size.type            = type_;
		descriptor_pool_size.descriptorCount = count_;
		return descriptor_pool_size;
	}

	inline VkDescriptorPoolCreateInfo descriptorPoolCreateInfo(const std::vector<VkDescriptorPoolSize>& pool_sizes_,
																														 uint32_t max_sets_)
	{
		VkDescriptorPoolCreateInfo descriptor_pool_info;
		descriptor_pool_info.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptor_pool_info.pNext         = nullptr;
		descriptor_pool_info.flags         = NULL;
		descriptor_pool_info.maxSets       = max_sets_;
		descriptor_pool_info.poolSizeCount = static_cast<uint32_t>(pool_sizes_.size());
		descriptor_pool_info.pPoolSizes    = pool_sizes_.data();
		return descriptor_pool_info;
	}

	inline VkDescriptorSetLayoutBinding descriptorSetLayoutBinding(uint32_t binding_,
																																 VkDescriptorType type_,
																																 VkShaderStageFlags flags_,
																																 uint32_t count_ = 1)
	{
		VkDescriptorSetLayoutBinding set_layout_binding;
		set_layout_binding.binding            = binding_;
		set_layout_binding.descriptorType     = type_;
		set_layout_binding.descriptorCount    = count_;
		set_layout_binding.stageFlags         = flags_;
		set_layout_binding.pImmutableSamplers = nullptr;
		return set_layout_binding;
	}

	inline VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo(const std::vector<VkDescriptorSetLayoutBinding>& bindings_)
	{
		VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info;
		descriptor_set_layout_create_info.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptor_set_layout_create_info.pNext        = nullptr;
		descriptor_set_layout_create_info.flags        = NULL;
		descriptor_set_layout_create_info.bindingCount = static_cast<uint32_t>(bindings_.size());
		descriptor_set_layout_create_info.pBindings    = bindings_.data();
		return descriptor_set_layout_create_info;
	}

	inline VkPipelineShaderStageCreateInfo pipelineShaderStageCreateInfo(VkShaderStageFlagBits stage_,
																																			 const VkShaderModule& module_)
	{
		VkPipelineShaderStageCreateInfo pipeline_shader_stage_create_info;
		pipeline_shader_stage_create_info.sType               = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		pipeline_shader_stage_create_info.pNext               = nullptr;
		pipeline_shader_stage_create_info.flags               = 0;
		pipeline_shader_stage_create_info.stage               = stage_;
		pipeline_shader_stage_create_info.module              = module_;
		pipeline_shader_stage_create_info.pName               = "main";
		pipeline_shader_stage_create_info.pSpecializationInfo = nullptr;
		return pipeline_shader_stage_create_info;
	}

	inline VkPipelineVertexInputStateCreateInfo pipelineVertexInputStageCreateInfo(const std::vector<VkVertexInputBindingDescription>& binding_description_,
																																								 const std::vector<VkVertexInputAttributeDescription>& attribute_description_)
	{
		VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info;
		vertex_input_state_create_info.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertex_input_state_create_info.pNext                           = nullptr;
		vertex_input_state_create_info.flags                           = 0;
		vertex_input_state_create_info.vertexBindingDescriptionCount   = static_cast<uint32_t>(binding_description_.size());
		vertex_input_state_create_info.pVertexBindingDescriptions      = binding_description_.data();
		vertex_input_state_create_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(attribute_description_.size());
		vertex_input_state_create_info.pVertexAttributeDescriptions    = attribute_description_.data();
		return vertex_input_state_create_info;
	}

	inline VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo()
	{
		VkPipelineInputAssemblyStateCreateInfo input_assembly_create_info;
		input_assembly_create_info.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		input_assembly_create_info.pNext                  = nullptr;
		input_assembly_create_info.flags                  = NULL;
		input_assembly_create_info.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		input_assembly_create_info.primitiveRestartEnable = VK_FALSE;
		return input_assembly_create_info;
	}

	inline VkPipelineViewportStateCreateInfo pipelineViewportStateCreateInfo(uint32_t viewport_count_,
																																					 uint32_t scissor_count_,
																																					 const std::vector<VkViewport>& viewports_ = std::vector<VkViewport>({}),
																																					 const std::vector<VkRect2D>& scissors_    = std::vector<VkRect2D>({}))
	{
		VkPipelineViewportStateCreateInfo viewport_state_create_info;
		viewport_state_create_info.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewport_state_create_info.pNext         = nullptr;
		viewport_state_create_info.flags         = NULL;
		viewport_state_create_info.viewportCount = viewport_count_;
		viewport_state_create_info.pViewports    = viewports_.data(); // nullptr because using dynamic state viewport
		viewport_state_create_info.scissorCount  = scissor_count_;
		viewport_state_create_info.pScissors     = scissors_.data(); // nullptr because using dynamic state scissor}
		return viewport_state_create_info;
	}

	inline VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo(VkPolygonMode polygon_mode_,
																																										 VkCullModeFlags cull_mode_,
																																										 VkFrontFace front_face_)
	{
		VkPipelineRasterizationStateCreateInfo rasterization_state_create_info;
		rasterization_state_create_info.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterization_state_create_info.pNext                   = nullptr;
		rasterization_state_create_info.flags                   = NULL;
		rasterization_state_create_info.depthClampEnable        = VK_FALSE;
		rasterization_state_create_info.rasterizerDiscardEnable = VK_FALSE;
		rasterization_state_create_info.polygonMode             = polygon_mode_;
		rasterization_state_create_info.cullMode                = cull_mode_;
		rasterization_state_create_info.frontFace               = front_face_;
		rasterization_state_create_info.depthBiasEnable         = VK_FALSE;
		rasterization_state_create_info.depthBiasConstantFactor = 0.0f;
		rasterization_state_create_info.depthBiasClamp          = 0.0f;
		rasterization_state_create_info.depthBiasSlopeFactor    = 0.0f;
		rasterization_state_create_info.lineWidth               = 1.0f;
		return rasterization_state_create_info;
	}

	inline VkPipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo(VkSampleCountFlagBits rasterization_samples_)
	{
		VkPipelineMultisampleStateCreateInfo multisample_state_create_info;
		multisample_state_create_info.sType                 = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisample_state_create_info.pNext                 = nullptr;
		multisample_state_create_info.flags                 = NULL;
		multisample_state_create_info.rasterizationSamples  = rasterization_samples_;
		multisample_state_create_info.sampleShadingEnable   = VK_FALSE;
		multisample_state_create_info.minSampleShading      = 1.0f;
		multisample_state_create_info.pSampleMask           = nullptr;
		multisample_state_create_info.alphaToCoverageEnable = VK_FALSE;
		multisample_state_create_info.alphaToOneEnable      = VK_FALSE;
		return multisample_state_create_info;
	}

	inline VkPipelineColorBlendAttachmentState pipelineColorBlendAttachmentState(VkBool32 blend_enable_,
																																							 VkColorComponentFlags color_write_mask_)
	{
		VkPipelineColorBlendAttachmentState color_blend_attachment_state;
		color_blend_attachment_state.blendEnable         = blend_enable_;
		color_blend_attachment_state.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
		color_blend_attachment_state.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
		color_blend_attachment_state.colorBlendOp        = VK_BLEND_OP_ADD;
		color_blend_attachment_state.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		color_blend_attachment_state.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		color_blend_attachment_state.alphaBlendOp        = VK_BLEND_OP_ADD;
		color_blend_attachment_state.colorWriteMask      = color_write_mask_;
		return color_blend_attachment_state;
	}

	inline VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo(const std::vector<VkPipelineColorBlendAttachmentState>& attachment_states_)
	{
		VkPipelineColorBlendStateCreateInfo color_blend_state_create_info;
		color_blend_state_create_info.sType             = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		color_blend_state_create_info.pNext             = nullptr;
		color_blend_state_create_info.flags             = NULL;
		color_blend_state_create_info.logicOpEnable     = VK_FALSE;
		color_blend_state_create_info.logicOp           = VK_LOGIC_OP_COPY;
		color_blend_state_create_info.attachmentCount   = static_cast<uint32_t>(attachment_states_.size());
		color_blend_state_create_info.pAttachments      = attachment_states_.data();
		color_blend_state_create_info.blendConstants[0] = 0.0f;
		color_blend_state_create_info.blendConstants[1] = 0.0f;
		color_blend_state_create_info.blendConstants[2] = 0.0f;
		color_blend_state_create_info.blendConstants[3] = 0.0f;
		return color_blend_state_create_info;
	}

	inline VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo(const std::vector<VkDynamicState>& dynamic_states_)
	{
		VkPipelineDynamicStateCreateInfo dynamic_state_create_info;
		dynamic_state_create_info.sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamic_state_create_info.pNext             = nullptr;
		dynamic_state_create_info.flags             = NULL;
		dynamic_state_create_info.dynamicStateCount = static_cast<uint32_t>(dynamic_states_.size());
		dynamic_state_create_info.pDynamicStates    = dynamic_states_.data();
		return dynamic_state_create_info;
	}

	inline VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo(const VkDescriptorSetLayout& set_layout_)
	{
		VkPipelineLayoutCreateInfo pipeline_layout_create_info;
		pipeline_layout_create_info.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipeline_layout_create_info.pNext                  = nullptr;
		pipeline_layout_create_info.flags                  = NULL;
		pipeline_layout_create_info.setLayoutCount         = 1;
		pipeline_layout_create_info.pSetLayouts            = &set_layout_;
		pipeline_layout_create_info.pushConstantRangeCount = 0;
		pipeline_layout_create_info.pPushConstantRanges    = nullptr;
		return pipeline_layout_create_info;
	}

	inline VkDescriptorSetAllocateInfo descriptorSetAllocateInfo(const VkDescriptorPool& descriptor_pool_,
																															 const std::vector<VkDescriptorSetLayout>& set_layouts_)
	{
		VkDescriptorSetAllocateInfo descriptor_set_allocate_info;
		descriptor_set_allocate_info.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		descriptor_set_allocate_info.pNext              = nullptr;
		descriptor_set_allocate_info.descriptorPool     = descriptor_pool_;
		descriptor_set_allocate_info.descriptorSetCount = static_cast<uint32_t>(set_layouts_.size());
		descriptor_set_allocate_info.pSetLayouts        = set_layouts_.data();
		return descriptor_set_allocate_info;
	}

	inline VkDescriptorBufferInfo descriptorBufferInfo(const VkBuffer& buffer_,
																										 uint32_t offset_,
																										 VkDeviceSize range_)
	{
		VkDescriptorBufferInfo buffer_info;
		buffer_info.buffer = buffer_;
		buffer_info.offset = offset_;
		buffer_info.range  = range_;
		return buffer_info;
	}

	inline VkWriteDescriptorSet writeDescriptorSet(const VkDescriptorSet& dst_set_,
																								 uint32_t binding_,
																								 uint32_t descriptor_count_,
																								 VkDescriptorType descriptor_type_,
																								 const VkDescriptorBufferInfo& buffer_info_)
	{
		VkWriteDescriptorSet write_descriptor_set;
		write_descriptor_set.sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write_descriptor_set.pNext            = nullptr;
		write_descriptor_set.dstSet           = dst_set_;
		write_descriptor_set.dstBinding       = binding_;
		write_descriptor_set.dstArrayElement  = 0;
		write_descriptor_set.descriptorCount  = descriptor_count_;
		write_descriptor_set.descriptorType   = descriptor_type_;
		write_descriptor_set.pImageInfo       = nullptr;
		write_descriptor_set.pBufferInfo      = &buffer_info_;
		write_descriptor_set.pTexelBufferView = nullptr;
		return write_descriptor_set;
	}

	inline VkCommandPoolCreateInfo commandPoolCreateInfo(VkCommandPoolCreateFlags flags_, uint32_t queue_index_)
	{
		VkCommandPoolCreateInfo command_pool_create_info;
		command_pool_create_info.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		command_pool_create_info.pNext            = nullptr;
		command_pool_create_info.flags            = flags_;
		command_pool_create_info.queueFamilyIndex = queue_index_;
		return command_pool_create_info;
	}
}
