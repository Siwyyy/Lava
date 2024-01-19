#pragma once
#include <vulkan/vulkan_core.h>

namespace lvc
{
	class Device;
	class Swapchain;

	class RenderPass
	{
	public:
		RenderPass(const Device* device,const Swapchain* swapchain);
		RenderPass() = delete;
		~RenderPass();

	private:
		const VkDevice& m_device;
		const VkFormat& m_swapchain_image_format;

		VkRenderPass m_render_pass;
	};
}
