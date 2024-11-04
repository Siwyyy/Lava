#pragma once
#include "Lava/Lavapch.h"

namespace Lava
{
	struct Vertex3Color;

	class BasicBody3D
	{
	public:
		BasicBody3D(const std::vector<Vertex3Color>& vertices_, const std::vector<uint32_t>& indices_);
		BasicBody3D(const std::vector<Vertex3Color>& vertices_, const std::vector<uint32_t>& indices_, const glm::mat4& transform_);
		BasicBody3D(const std::vector<Vertex3Color>& vertices_, const std::vector<uint32_t>& indices_, const glm::mat4& transform_, const glm::mat4& rotation_);
		~BasicBody3D();

		glm::mat4 transform;
		glm::mat4 rotation;

		std::vector<Vertex3Color> vertices;
		std::vector<uint32_t> indices;

		struct State
		{
			bool ready_to_copy = false;
			bool ready_to_draw = false;
		} state;

	public:
		void loadStgBuffers();
		void copyStgBuffersToGpu(const VkCommandPool& copy_command_pool_);
		void drawIndexed(const VkCommandBuffer& command_buffer_);

	private:
		VkBuffer m_vertex_staging_buffer;
		VkBuffer m_vertex_buffer;
		void* m_vertex_staging_buffer_memory_mapped;
		VkDeviceMemory m_vertex_staging_buffer_memory;
		VkDeviceMemory m_vertex_buffer_memory;

		VkBuffer m_index_staging_buffer;
		VkDeviceMemory m_index_staging_buffer_memory;
		void* m_index_staging_buffer_memory_mapped;
		VkBuffer m_index_buffer;
		VkDeviceMemory m_index_buffer_memory;

	private:
		void init();
	};
}
