#pragma once

#include <vector>
#include <vulkan/vulkan.h>

namespace lvc
{
	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> present_modes;
	};

	class SwapChain
	{
	public:
	private:
	};
}
 