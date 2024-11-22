#pragma once

#include "Lava/Lavapch.h"
#include "Lava/Renderer/Vertex.h"

namespace Lava::Components
{
	class Transform;
}

namespace Lava::Components
{
	class BasicBody3D
	{
	public:
		BasicBody3D(const std::vector<Vertex3Color>& vertices_,
								const std::vector<uint32_t>& indices_);

		virtual ~BasicBody3D();

		void loadStgBuffers();
		void copyStgBuffersToGpu(const VkCommandPool& copy_command_pool_);
		void drawIndexed(const VkCommandBuffer& command_buffer_) const;

		struct Data
		{
			bool ready_to_copy = false;
			bool ready_to_draw = false;
		} basic_body_3d;

		Transform* transform_data = nullptr;

	private:
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
