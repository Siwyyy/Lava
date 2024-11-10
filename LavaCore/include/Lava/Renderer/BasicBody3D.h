#pragma once
#include "Lava/Lavapch.h"
#include <glm/ext/matrix_transform.hpp>

namespace Lava
{
	struct Vertex3Color;

	class BasicBody3D
	{
	public:
		BasicBody3D(const std::vector<Vertex3Color>& vertices_,
								const std::vector<uint32_t>& indices_,
								const glm::vec3& transform_ = glm::vec3(0, 0, 0),
								const glm::mat4& rotation_  = glm::mat4(1.0f));
		~BasicBody3D();

		inline glm::mat4 getTranslationMatrix() const { return translate(glm::mat4(1.0f), m_position); }
		inline glm::mat4 getRotationMatrix() const { return m_rotation; }

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
		glm::vec3 m_position;
		glm::mat4 m_rotation;

		std::vector<Vertex3Color> m_vertices;
		std::vector<uint32_t> m_indices;

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
	};
}
