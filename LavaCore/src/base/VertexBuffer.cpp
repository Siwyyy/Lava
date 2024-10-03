#include "VertexBuffer.hpp"
#include "Vertex.hpp"

#include <stdexcept>

using namespace lava;

VertexBuffer::VertexBuffer(const VkDevice& t_device,
													 const VkPhysicalDevice& t_physical_device)
	: m_device(t_device)
	, m_physical_device(t_physical_device)
{
	VkBufferCreateInfo buffer_create_info;
	buffer_create_info.sType                 = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	buffer_create_info.pNext                 = nullptr;
	buffer_create_info.flags                 = NULL;
	buffer_create_info.size                  = sizeof(VERTICES[0]) * VERTICES.size();
	buffer_create_info.usage                 = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	buffer_create_info.sharingMode           = VK_SHARING_MODE_EXCLUSIVE;
	buffer_create_info.queueFamilyIndexCount = 0;
	buffer_create_info.pQueueFamilyIndices   = nullptr;

	if (vkCreateBuffer(m_device, &buffer_create_info, nullptr, &m_vertex_buffer) != VK_SUCCESS)
		throw std::runtime_error("err: failed to create vertex buffer!\n");

	VkMemoryRequirements memory_requirements;
	vkGetBufferMemoryRequirements(m_device, m_vertex_buffer, &memory_requirements);

	VkMemoryAllocateInfo allocate_info;
	allocate_info.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocate_info.pNext           = nullptr;
	allocate_info.allocationSize  = memory_requirements.size;
	allocate_info.memoryTypeIndex = findMemoryType(memory_requirements.memoryTypeBits,
																								 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
																								 VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	if (vkAllocateMemory(m_device, &allocate_info, nullptr, &m_vertex_buffer_memory) != VK_SUCCESS)
		throw std::runtime_error("err: failed to allocate vertex buffer memory!\n");

	vkBindBufferMemory(m_device, m_vertex_buffer, m_vertex_buffer_memory, 0);

	void* data;
	vkMapMemory(m_device, m_vertex_buffer_memory, 0, buffer_create_info.size,NULL, &data);
	memcpy(data, VERTICES.data(), (size_t)buffer_create_info.size);
	vkUnmapMemory(m_device, m_vertex_buffer_memory);
}

VertexBuffer::~VertexBuffer()
{
	vkDestroyBuffer(m_device, m_vertex_buffer, nullptr);
	vkFreeMemory(m_device, m_vertex_buffer_memory, nullptr);
}

uint32_t VertexBuffer::findMemoryType(const uint32_t t_type_filter, const VkMemoryPropertyFlags t_property_flags) const
{
	VkPhysicalDeviceMemoryProperties properties;
	vkGetPhysicalDeviceMemoryProperties(m_physical_device, &properties);
	for (uint32_t i = 0; i < properties.memoryTypeCount; i++)
		if ((t_type_filter & (1 << i)) &&
				(properties.memoryTypes[i].propertyFlags & t_property_flags) == t_property_flags)
			return i;
	throw std::runtime_error("err: failed to find memory type!\n");
};
