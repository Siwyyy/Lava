#pragma once

#include "Lava/Renderer/ResourceBuffer/StorageBuffer.h"
#include "Lava/Renderer/ResourceBuffer/UniformBuffer.h"

namespace Lava
{
	namespace ObjectSystem
	{
		class CMesh;
		class CCamera;
	}

	class GraphicsContext;

	class Pipeline
	{
	public:
		Pipeline();
		~Pipeline();

		void bind() const;
		void updateResourceBuffers(uint32_t current_frame_);

		void registerMesh(ObjectSystem::CMesh* mesh_);
		void registerMesh(const std::vector<ObjectSystem::CMesh*>& meshes_);

		inline void setCamera(const std::shared_ptr<ObjectSystem::CCamera>& camera_) { m_camera = camera_; }

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

		// Camera Uniform Buffer
		struct CameraData
		{
			glm::mat4 view;
			glm::mat4 projection;
			glm::mat4 projection_view;

			inline void calculateProjectionView() { projection_view = projection * view; }
		} m_camera_data;

		std::vector<UniformBuffer<CameraData>> m_camera_uniform_buffers;

		// Model Dynamic Uniform Buffer
		struct ModelData
		{
			glm::mat4 transform;
			glm::mat4 rotation;
		};

		std::vector<ModelData> m_model_data = std::vector<ModelData>(1024);

		std::vector<StorageBuffer<ModelData>> m_model_storage_buffers;

		std::vector<ObjectSystem::CMesh*> m_meshes;
		std::shared_ptr<ObjectSystem::CCamera> m_camera = nullptr;

	private:
		void createVulkanDescriptorSetLayout();
		void createVulkanGraphicsPipeline();

		void createVulkanCommandPool();

		void createShaderDataBuffers();

		void createVulkanDescriptorPool();
		void createVulkanDescriptorSets();

		std::vector<char> readShaderFile(const std::string& filename_) const;
		VkShaderModule createShaderModule(const std::vector<char>& code_) const;
	};
}
