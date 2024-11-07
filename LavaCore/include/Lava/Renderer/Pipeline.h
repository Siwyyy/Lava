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
		} m_static_uniform;

		std::vector<VkBuffer> m_uniform_buffers;
		std::vector<VkDeviceMemory> m_uniform_buffers_memory;
		std::vector<void*> m_uniform_buffers_mapped;

		// Dynamic Uniform Buffer
		struct DynamicUniformBufferObject
		{
			glm::mat4 transform;
		} m_dynamic_uniform;

		std::vector<std::shared_ptr<BasicBody3D>> m_objects;

	private:
		void createVulkanDescriptorSetLayout();
		void createVulkanGraphicsPipeline();

		void createVulkanCommandPool();

		void createStaticUniformBuffers();
		void createDynamicUniformBuffer();

		void createVulkanDescriptorPool();
		void createVulkanDescriptorSets();

		std::vector<char> readShaderFile(const std::string& filename_);
		VkShaderModule createShaderModule(const std::vector<char>& code_) const;
	};
}
