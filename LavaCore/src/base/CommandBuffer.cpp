#include "CommandBuffer.hpp"

#include <stdexcept>

using namespace lava;

CommandBuffer::CommandBuffer(const VkCommandPool& t_command_pool,
														 const VkDevice& t_device,
														 const VkRenderPass& t_render_pass,
														 const std::vector<VkFramebuffer>& t_framebuffers,
														 const VkPipeline& t_pipeline,
														 const VkExtent2D& t_extent_2d,
														 const int t_max_frames_in_flight,
														 const VkBuffer& t_vertex_buffer)
	: m_command_pool(t_command_pool)
	, m_device(t_device)
	, m_render_pass(t_render_pass)
	, m_framebuffers(t_framebuffers)
	, m_pipeline(t_pipeline)
	, m_extent_2d(t_extent_2d)
	, m_vertex_buffer(t_vertex_buffer)
{
	m_command_buffers.resize(t_max_frames_in_flight);

	VkCommandBufferAllocateInfo command_buffer_allocate_info;
	command_buffer_allocate_info.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	command_buffer_allocate_info.commandPool        = m_command_pool;
	command_buffer_allocate_info.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	command_buffer_allocate_info.commandBufferCount = static_cast<uint32_t>(m_command_buffers.size());
	command_buffer_allocate_info.pNext              = nullptr;

	if (vkAllocateCommandBuffers(m_device, &command_buffer_allocate_info, m_command_buffers.data()) != VK_SUCCESS)
		throw std::runtime_error("err: Failed to allocate command buffer!\n");
}

void CommandBuffer::recordCommandBuffer(const uint32_t t_command_buffer_index, const uint32_t t_image_index) const
{
	VkCommandBufferBeginInfo command_buffer_begin_info;
	command_buffer_begin_info.sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	command_buffer_begin_info.flags            = 0;
	command_buffer_begin_info.pInheritanceInfo = nullptr;
	command_buffer_begin_info.pNext            = nullptr;

	if (vkBeginCommandBuffer(m_command_buffers[t_command_buffer_index], &command_buffer_begin_info) != VK_SUCCESS)
		throw std::runtime_error("err: Failed to begin command buffer recording");

	const VkClearValue clear_value = {{{0.0f,0.0f,0.0f,0.0f}}};
	VkRenderPassBeginInfo render_pass_begin_info;
	render_pass_begin_info.sType             = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	render_pass_begin_info.renderPass        = m_render_pass;
	render_pass_begin_info.framebuffer       = m_framebuffers[t_image_index];
	render_pass_begin_info.renderArea.offset = {0,0};
	render_pass_begin_info.renderArea.extent = m_extent_2d;
	render_pass_begin_info.clearValueCount   = 1;
	render_pass_begin_info.pClearValues      = &clear_value;
	render_pass_begin_info.pNext             = nullptr;
	vkCmdBeginRenderPass(m_command_buffers[t_command_buffer_index], &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(m_command_buffers[t_command_buffer_index], VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);

	VkViewport viewport;
	viewport.x        = 0.0f;
	viewport.y        = 0.0f;
	viewport.width    = static_cast<float>(m_extent_2d.width);
	viewport.height   = static_cast<float>(m_extent_2d.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(m_command_buffers[t_command_buffer_index], 0, 1, &viewport);

	VkRect2D scissor;
	scissor.offset = {0,0};
	scissor.extent = m_extent_2d;
	vkCmdSetScissor(m_command_buffers[t_command_buffer_index], 0, 1, &scissor);

	const VkBuffer vertex_buffers[] = {m_vertex_buffer};
	const VkDeviceSize offsets[]    = {0};
	vkCmdBindVertexBuffers(m_command_buffers[t_command_buffer_index], 0, 1, vertex_buffers, offsets);

	vkCmdDraw(m_command_buffers[t_command_buffer_index], 3, 1, 0, 0);

	vkCmdEndRenderPass(m_command_buffers[t_command_buffer_index]);

	if (vkEndCommandBuffer(m_command_buffers[t_command_buffer_index]) != VK_SUCCESS)
		throw std::runtime_error("err: Failed to record command buffer!\n");
}
