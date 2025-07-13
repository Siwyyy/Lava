#pragma once
#include "Lava/LavaPCH.h"

#include "AActor.h"
#include "CComponent.h"
#include "CTransform.h"

#include "Lava/Renderer/Vertex.h"

namespace Lava::ObjectSystem
{
	class CMesh final : public CComponent
	{
	public:
		CMesh(const std::string& component_name_,
					const std::vector<Vertex3Color>& vertices_,
					const std::vector<uint32_t>& indices_);

		~CMesh() override;

		void initialize() override { CComponent::initialize(); }
		void tick(float delta_time_) override { CComponent::tick(delta_time_); }

		void load(const VkCommandPool& copy_command_pool_);
		void drawIndexed(const VkCommandBuffer& command_buffer_, uint32_t instance_index_ = 0) const;

		inline glm::mat4 getTranslationMatrix() const { return getOwner()->getComponent<CTransform>()->getTranslationMatrix(); }
		inline glm::mat4 getRotationMatrix() const { return getOwner()->getComponent<CTransform>()->getRotationMatrix(); }

		inline bool isLoaded() const { return m_loaded; }

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
