#pragma once

#include "Lava/Lavapch.h"
#include "Lava/Renderer/Vertex.h"

namespace Lava::Components
{
	class Transform;
}

namespace Lava::Components
{
	class Mesh
	{
	public:
		Mesh(const std::shared_ptr<Transform>& transform_,
				 const std::vector<Vertex3Color>& vertices_,
				 const std::vector<uint32_t>& indices_);

		~Mesh();

		void load(const VkCommandPool& copy_command_pool_);
		void drawIndexed(const VkCommandBuffer& command_buffer_, uint32_t instance_index_ = 0) const;

		inline bool isLoaded() const { return m_loaded; }

		const std::shared_ptr<Transform> object_transform = nullptr;

	private:
		std::vector<Vertex3Color> m_vertices;
		std::vector<uint32_t> m_indices;

		// Vertex Buffer
		VkBuffer m_vertex_staging_buffer;
		VkDeviceMemory m_vertex_staging_buffer_memory;
		void* m_vertex_staging_buffer_memory_mapped;

		VkBuffer m_vertex_buffer;
		VkDeviceMemory m_vertex_buffer_memory;

		// Index Buffer
		VkBuffer m_index_staging_buffer;
		VkDeviceMemory m_index_staging_buffer_memory;
		void* m_index_staging_buffer_memory_mapped;

		VkBuffer m_index_buffer;
		VkDeviceMemory m_index_buffer_memory;

		bool m_loaded = false;
	};
}
