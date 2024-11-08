#pragma once

namespace Lava
{
	class BasicBody3D;
	class GraphicsContext;

	class Pipeline
	{
	public:
		Pipeline();
		~Pipeline();

		void draw(const VkCommandBuffer& command_buffer_, uint32_t current_frame_) const;
		void updateVulkanUniformBuffer(uint32_t current_frame_);

		void pushObjects(const std::shared_ptr<BasicBody3D>& object_);
		void pushObjects(const std::vector<std::shared_ptr<BasicBody3D>>& objects_);

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
		VkCommandPool m_copy_command_pool;

		// Static Uniform Buffer
		struct StaticUniformBufferObject
		{
			glm::mat4 model;
			glm::mat4 view;
			glm::mat4 proj;
		} m_static_uniform_object;

		struct StaticUniformBuffer
		{
			std::vector<VkBuffer> buffers;
			std::vector<VkDeviceMemory> buffers_memory;
			std::vector<void*> buffers_mapped;
		} m_static_uniform_buffer;

		// Dynamic Uniform Buffer
		struct DynamicUniformBufferObject
		{
			glm::mat4* transform = nullptr;
		} m_dynamic_uniform_object;

		struct DynamicUniformBuffer
		{
			std::vector<VkBuffer> buffers;
			std::vector<VkDeviceMemory> buffers_memory;
			std::vector<void*> buffers_mapped;
			const uint32_t num_objects = 128;
			VkDeviceSize aligned_object_size;
			VkDeviceSize buffer_size;
		} m_dynamic_uniform_buffer;

		std::vector<std::shared_ptr<BasicBody3D>> m_objects;

	private:
		void createVulkanDescriptorSetLayout();
		void createVulkanGraphicsPipeline();

		void createVulkanCommandPool();

		void createUniformBuffers();

		void createVulkanDescriptorPool();
		void createVulkanDescriptorSets();

		std::vector<char> readShaderFile(const std::string& filename_);
		VkShaderModule createShaderModule(const std::vector<char>& code_) const;
	};
}
