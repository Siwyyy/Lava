#pragma once

namespace Lava
{
	namespace Components {
		class BasicBody3D;
	}

	class Camera3D;
	class GraphicsContext;

	class Pipeline
	{
	public:
		Pipeline();
		~Pipeline();

		void draw(const VkCommandBuffer& command_buffer_, uint32_t current_frame_) const;
		void updateVulkanUniformBuffer(uint32_t current_frame_);

		void pushObjects(const std::shared_ptr<Components::BasicBody3D>& object_);
		void pushObjects(const std::vector<std::shared_ptr<Components::BasicBody3D>>& objects_);

		inline void setCamera(const std::shared_ptr<Camera3D>& camera_) { m_camera = camera_; }

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
			glm::mat4 view;
			glm::mat4 projection;
			glm::mat4 projection_view;

			inline void calculateProjectionView() { projection_view = projection * view; }
		} m_camera_ubo_data;

		struct StaticUniformBuffer
		{
			std::vector<VkBuffer> buffers;
			std::vector<VkDeviceMemory> buffers_memory;
			std::vector<void*> buffers_mapped;
		} m_camera_uniform;

		// Dynamic Uniform Buffer
		struct DynamicUniformBufferObject
		{
			glm::mat4 transform;
			glm::mat4 rotation;
		}* m_model_ubo_data = nullptr;

		struct DynamicUniformBuffer
		{
			std::vector<VkBuffer> buffers;
			std::vector<VkDeviceMemory> buffers_memory;
			std::vector<void*> buffers_mapped;
			const uint32_t num_objects = 128;
			VkDeviceSize alignment;
			VkDeviceSize buffer_size;
		} m_model_uniform;

		std::vector<std::shared_ptr<Components::BasicBody3D>> m_objects;
		std::shared_ptr<Camera3D> m_camera = nullptr;

	private:
		void createVulkanDescriptorSetLayout();
		void createVulkanGraphicsPipeline();

		void createVulkanCommandPool();

		void createStaticUniformBuffers();
		void createDynamicUniformBuffers();

		void createVulkanDescriptorPool();
		void createVulkanDescriptorSets();

		std::vector<char> readShaderFile(const std::string& filename_);
		VkShaderModule createShaderModule(const std::vector<char>& code_) const;
	};
}
