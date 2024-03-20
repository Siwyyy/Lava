#include "lvc/CommandBuffer.hpp"

#include "lvc/CommandPool.hpp"
#include "lvc/Device.hpp"
#include "lvc/GraphicsPipeline.hpp"
#include "lvc/RenderPass.hpp"
#include "lvc/Swapchain.hpp"

#include <stdexcept>

using namespace lvc;

CommandBuffer::CommandBuffer(const CommandPool* t_command_pool,
														 const Device* t_device,
														 const RenderPass* t_render_pass,
														 const Swapchain* t_swapchain,
														 const GraphicsPipeline* t_graphics_pipeline)
	: m_command_pool(t_command_pool->hCommandPool())
	, m_device(t_device->hVkDevice())
	, m_render_pass(t_render_pass->hRenderPass())
	, m_framebuffers(t_render_pass->hFramebuffers())
	, m_pipeline(t_graphics_pipeline->hPipeline())
	, m_extent_2d(t_swapchain->hExtent2d())
{
	VkCommandBufferAllocateInfo command_buffer_allocate_info;
	command_buffer_allocate_info.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	command_buffer_allocate_info.commandPool        = m_command_pool;
	command_buffer_allocate_info.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	command_buffer_allocate_info.commandBufferCount = 1;
	command_buffer_allocate_info.pNext              = nullptr;

	if (vkAllocateCommandBuffers(m_device, &command_buffer_allocate_info, &m_command_buffer) != VK_SUCCESS)
		throw std::runtime_error("err: Failed to allocate command buffer!\n");
}

void CommandBuffer::recordCommandBuffer(const uint32_t t_image_index) const
{
	VkCommandBufferBeginInfo command_buffer_begin_info;
	command_buffer_begin_info.sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	command_buffer_begin_info.flags            = 0;
	command_buffer_begin_info.pInheritanceInfo = nullptr;
	command_buffer_begin_info.pNext            = nullptr;

	if (vkBeginCommandBuffer(m_command_buffer, &command_buffer_begin_info) != VK_SUCCESS)
		throw std::runtime_error("err: Failed to begin command buffer recording");

	VkRenderPassBeginInfo render_pass_begin_info;
	render_pass_begin_info.sType             = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	render_pass_begin_info.renderPass        = m_render_pass;
	render_pass_begin_info.framebuffer       = m_framebuffers[t_image_index];
	render_pass_begin_info.renderArea.offset = {0,0};
	render_pass_begin_info.renderArea.extent = m_extent_2d;
	const VkClearValue clear_value           = {{{0.0f,0.0f,0.0f,0.0f}}};
	render_pass_begin_info.clearValueCount   = 1;
	render_pass_begin_info.pClearValues      = &clear_value;
	vkCmdBeginRenderPass(m_command_buffer, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
	
	vkCmdBindPipeline(m_command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);

	VkViewport viewport;
	viewport.x        = 0.0f;
	viewport.y        = 0.0f;
	viewport.width    = static_cast<float>(m_extent_2d.width);
	viewport.height   = static_cast<float>(m_extent_2d.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(m_command_buffer, 0, 1, &viewport);

	VkRect2D scissor;
	scissor.offset = {0,0};
	scissor.extent = m_extent_2d;
	vkCmdSetScissor(m_command_buffer, 0, 1, &scissor);

	vkCmdDraw(m_command_buffer, 3, 1, 0, 0);

	vkCmdEndRenderPass(m_command_buffer);

	if (vkEndCommandBuffer(m_command_buffer) != VK_SUCCESS)
		throw std::runtime_error("err: Failed to record command buffer!\n");
}
