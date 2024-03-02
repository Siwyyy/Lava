#pragma once

#include <vulkan/vulkan_core.h>

#include <vector>

namespace lvc
{
	class Device;
	class Swapchain;

	class RenderPass
	{
	public:
		RenderPass(const Device* device, const Swapchain* swapchain);
		RenderPass() = delete;
		~RenderPass();

		inline VkRenderPass& rHandle() { return m_render_pass; }

		void createFrameBuffers();

	private:
		const VkDevice& m_device;
		const VkExtent2D& m_swapchain_extent_2d;
		const VkFormat& m_swapchain_image_format;
		const std::vector<VkImageView>& m_image_views;

		VkRenderPass m_render_pass;

		std::vector<VkFramebuffer> m_framebuffers;
	};
}
