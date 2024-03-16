#pragma once

#include <vulkan/vulkan.h>

#include <vector>

namespace lvc
{
	class CommandPool;
	class GraphicsPipeline;
	class RenderPass;
	class Swapchain;
	class Device;

	class CommandBuffer
	{
	public:
		CommandBuffer(const CommandPool* t_command_pool,
									const Device* t_device,
									const RenderPass* t_render_pass,
									const Swapchain* t_swapchain,
									const GraphicsPipeline* t_graphics_pipeline);

		inline VkCommandBuffer& hCommandBuffer() { return m_command_buffer; }

		void recordCommandBuffer(uint32_t t_image_index) const;

	private:
		const VkCommandPool& m_command_pool;
		const VkDevice& m_device;
		const VkRenderPass& m_render_pass;
		const std::vector<VkFramebuffer>& m_framebuffers;
		const VkPipeline& m_pipeline;
		const VkExtent2D& m_extent_2d;

		VkCommandBuffer m_command_buffer;
	};
}
