#pragma once

#include <vector>
#include <vulkan/vulkan.h>

namespace lvc
{
	class Device;
	class Window;

	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> present_modes;
	};

	class SwapChain
	{
	public:
		SwapChain(const Device* device, const Window* window);
		~SwapChain() = default;

	private:
		const Device* m_device;
		const Window* m_window;
	};
}
