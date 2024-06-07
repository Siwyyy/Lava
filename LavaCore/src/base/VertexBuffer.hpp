#pragma once

#include <vulkan/vulkan.h>

namespace lvc
{
	class VertexBuffer
	{
	public:
		explicit VertexBuffer(const VkDevice& t_device,
													const VkPhysicalDevice& t_physical_device);
		~VertexBuffer();

		inline const VkBuffer& hVertexBuffer() const { return m_vertex_buffer; }

	private:
		const VkDevice& m_device;
		const VkPhysicalDevice& m_physical_device;

		VkBuffer m_vertex_buffer;
		VkDeviceMemory m_vertex_buffer_memory;

		uint32_t findMemoryType(uint32_t t_type_filter, VkMemoryPropertyFlags t_property_flags) const;
	};
}
