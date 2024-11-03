#pragma once
#include "Vertex3Color.h"

namespace Lava
{
	class GraphicsContext;

	class Pipeline
	{
	public:
		Pipeline();
		~Pipeline();

		void draw(const VkCommandBuffer& command_buffer_, uint32_t current_frame_);
		void updateVulkanUniformBuffer(uint32_t current_frame_);

	private:
		std::shared_ptr<GraphicsContext> m_context;

		// VkPipeline
		VkPipeline m_pipeline;
		VkPipelineLayout m_pipeline_layout;
		VkDescriptorSetLayout m_descriptor_set_layout;
		VkDescriptorPool m_descriptor_pool;
		std::vector<VkDescriptorSet> m_descriptor_sets;
		VkShaderModule m_vert_shader_module;
		VkShaderModule m_frag_shader_module;

		// Command pool for command buffers for copying buffers
		VkCommandPool m_command_pool;

		// BUFFERS
		VkBuffer m_vertex_buffer;
		VkDeviceMemory m_vertex_buffer_memory;
		VkBuffer m_index_buffer;
		VkDeviceMemory m_index_buffer_memory;

		// BUFFERS 2 TEST
		VkBuffer m_vertex_buffer2;
		VkDeviceMemory m_vertex_buffer_memory2;
		VkBuffer m_index_buffer2;
		VkDeviceMemory m_index_buffer_memory2;

		// Uniform buffers
		std::vector<VkBuffer> m_uniform_buffers;
		std::vector<VkDeviceMemory> m_uniform_buffers_memory;
		std::vector<void*> m_uniform_buffers_mapped;

	private:
		void createVulkanDescriptorSetLayout();
		void createVulkanGraphicsPipeline();

		void createVulkanCommandPool();

		void createVulkanVertexBuffer(const std::vector<Vertex3Color>& vertices_, VkBuffer& buffer_, VkDeviceMemory& memory_);
		void createVulkanIndexBuffer(const std::vector<uint16_t>& indices_, VkBuffer& buffer_, VkDeviceMemory& memory_);

		void createVulkanUniformBuffers();

		void createVulkanDescriptorPool();
		void createVulkanDescriptorSets();

		std::vector<char> readShaderFile(const std::string& filename_);
		VkShaderModule createShaderModule(const std::vector<char>& code_) const;

		void createVulkanBuffer(VkDeviceSize size_,
														VkBufferUsageFlags usage_,
														VkMemoryPropertyFlags props_,
														VkBuffer& buffer_,
														VkDeviceMemory& buffer_memory_);
		void copyVulkanBuffer(VkBuffer src_buffer_,
													VkBuffer dst_buffer_,
													VkDeviceSize size_);
	};
}
