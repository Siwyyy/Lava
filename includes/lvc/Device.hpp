#pragma once

#include "lvc/Instance.hpp"
#include "lvc/Window.hpp"

#include <vector>
#include <vulkan/vulkan.h>

namespace lvc
{
	class Device
	{
	public:
		Device(const Instance& instance,
			   const Window& window);
		~Device();

		inline const VkPhysicalDevice physical() const { return m_physical; }

	private:
		VkPhysicalDevice m_physical;
		const lvc::Instance m_instance;
		const lvc::Window m_window;

		void pickPhysicalDevice();
		int rateDeviceSuitability(VkPhysicalDevice device);
		bool isDeviceSuitable(const VkPhysicalDevice& device);
	};
}
