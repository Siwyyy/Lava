#include "Lava/Lavapch.h"
#include "Lava/Renderer/BasicBody3DModel.h"

#include "Lava/Application.h"
#include "Lava/Renderer/Buffers.h"
#include "Lava/Renderer/Vertex3Color.h"

using namespace Lava;

BasicBody3D::BasicBody3D(const std::vector<Vertex3Color>& vertices_, const std::vector<uint32_t>& indices_)
	: transform({0,0,0,0})
	, rotation({0,0,0,0})
	, vertices(vertices_)
	, indices(indices_)
{
	init();
}

BasicBody3D::BasicBody3D(const std::vector<Vertex3Color>& vertices_, const std::vector<uint32_t>& indices_, const glm::mat4& transform_)
	: transform(transform_)
	, rotation({0,0,0,0})
	, vertices(vertices_)
	, indices(indices_)
{
	init();
}

BasicBody3D::BasicBody3D(const std::vector<Vertex3Color>& vertices_, const std::vector<uint32_t>& indices_, const glm::mat4& transform_, const glm::mat4& rotation_)
	: transform(transform_)
	, rotation(rotation_)
	, vertices(vertices_)
	, indices(indices_)
{
	init();
}

BasicBody3D::~BasicBody3D()
{
	auto context = Application::getInstance().getWindow().getContext();

	vkDestroyBuffer(context->getDevice(), m_vertex_staging_buffer, nullptr);
	vkFreeMemory(context->getDevice(), m_vertex_staging_buffer_memory, nullptr);
	vkDestroyBuffer(context->getDevice(), m_index_staging_buffer, nullptr);
	vkFreeMemory(context->getDevice(), m_index_staging_buffer_memory, nullptr);

	vkDestroyBuffer(context->getDevice(), m_index_buffer, nullptr);
	vkFreeMemory(context->getDevice(), m_index_buffer_memory, nullptr);
	vkDestroyBuffer(context->getDevice(), m_vertex_buffer, nullptr);
	vkFreeMemory(context->getDevice(), m_vertex_buffer_memory, nullptr);
}

void BasicBody3D::loadStgBuffers()
{
	memcpy(m_vertex_staging_buffer_memory_mapped, vertices.data(), sizeof(vertices[0]) * vertices.size());
	memcpy(m_index_staging_buffer_memory_mapped, indices.data(), sizeof(indices[0]) * indices.size());
	state.ready_to_copy = true;
}

void BasicBody3D::copyStgBuffersToGpu(const VkCommandPool& copy_command_pool_)
{
	auto context = Application::getInstance().getWindow().getContext();
	VkCommandBufferAllocateInfo command_buffer_allocate_info{};
	command_buffer_allocate_info.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	command_buffer_allocate_info.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	command_buffer_allocate_info.commandPool        = copy_command_pool_;
	command_buffer_allocate_info.commandBufferCount = 1;

	VkCommandBuffer command_buffer;
	vkAllocateCommandBuffers(context->getDevice(), &command_buffer_allocate_info, &command_buffer);

	VkCommandBufferBeginInfo command_buffer_begin_info;
	command_buffer_begin_info.sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	command_buffer_begin_info.pNext            = nullptr;
	command_buffer_begin_info.flags            = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	command_buffer_begin_info.pInheritanceInfo = nullptr;

	vkBeginCommandBuffer(command_buffer, &command_buffer_begin_info);

	VkBufferCopy copy_region_vertex;
	copy_region_vertex.srcOffset = 0;
	copy_region_vertex.dstOffset = 0;
	copy_region_vertex.size      = sizeof(vertices[0]) * vertices.size();
	vkCmdCopyBuffer(command_buffer, m_vertex_staging_buffer, m_vertex_buffer, 1, &copy_region_vertex);

	VkBufferCopy copy_region_index;
	copy_region_index.srcOffset = 0;
	copy_region_index.dstOffset = 0;
	copy_region_index.size      = sizeof(indices[0]) * indices.size();
	vkCmdCopyBuffer(command_buffer, m_index_staging_buffer, m_index_buffer, 1, &copy_region_index);

	vkEndCommandBuffer(command_buffer);

	VkSubmitInfo submit_info;
	submit_info.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.pNext                = nullptr;
	submit_info.waitSemaphoreCount   = 0;
	submit_info.pWaitSemaphores      = nullptr;
	submit_info.pWaitDstStageMask    = nullptr;
	submit_info.commandBufferCount   = 1;
	submit_info.pCommandBuffers      = &command_buffer;
	submit_info.signalSemaphoreCount = 0;
	submit_info.pSignalSemaphores    = nullptr;

	vkQueueSubmit(context->getGraphicsQueue(), 1, &submit_info,VK_NULL_HANDLE);
	vkQueueWaitIdle(context->getGraphicsQueue());

	vkFreeCommandBuffers(context->getDevice(), copy_command_pool_, 1, &command_buffer);

	state.ready_to_draw = true;
}

void BasicBody3D::drawIndexed(const VkCommandBuffer& command_buffer_)
{
	const VkBuffer vertex_buffers[] = {m_vertex_buffer};
	const VkDeviceSize offsets[]    = {0};
	vkCmdBindVertexBuffers(command_buffer_, 0, 1, vertex_buffers, offsets);
	vkCmdBindIndexBuffer(command_buffer_, m_index_buffer, 0, VK_INDEX_TYPE_UINT32);
	vkCmdDrawIndexed(command_buffer_, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
}

void BasicBody3D::init()
{
	auto context = Application::getInstance().getWindow().getContext();
	// init vertex buffers
	VkDeviceSize vertex_buffer_size = sizeof(vertices[0]) * vertices.size();
	Buffers::createVulkanBuffer(vertex_buffer_size,
															VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
															VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
															m_vertex_staging_buffer,
															m_vertex_staging_buffer_memory);
	vkMapMemory(context->getDevice(), m_vertex_staging_buffer_memory, 0, vertex_buffer_size, 0, &m_vertex_staging_buffer_memory_mapped);
	Buffers::createVulkanBuffer(vertex_buffer_size,
															VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
															VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
															m_vertex_buffer,
															m_vertex_buffer_memory);

	// init index buffers
	VkDeviceSize index_buffer_size = sizeof(indices[0]) * indices.size();
	Buffers::createVulkanBuffer(index_buffer_size,
															VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
															VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
															m_index_staging_buffer,
															m_index_staging_buffer_memory);
	vkMapMemory(context->getDevice(), m_index_staging_buffer_memory, 0, index_buffer_size, 0, &m_index_staging_buffer_memory_mapped);
	Buffers::createVulkanBuffer(vertex_buffer_size,
															VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
															VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
															m_index_buffer,
															m_index_buffer_memory);

	loadStgBuffers();
}
