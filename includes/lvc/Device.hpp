#pragma once

#include "lvc/Instance.hpp"
#include "lvc/QueueFamilyIndices.hpp"
#include "lvc/Window.hpp"

#include <vector>
#include <vulkan/vulkan.h>

namespace lvc
{
	class Device
	{
	public:
		Device(const Instance* instance, const Window* window, const std::vector<const char*>& extensions);
		~Device();

		inline VkPhysicalDevice physical() const { return m_physical; }
		inline VkDevice logical() const { return m_logical; }
		inline const QueueFamilyIndices& queueFamilies() const { return m_family_indices; }

	private:
		VkPhysicalDevice m_physical;
		VkDevice m_logical;

		QueueFamilyIndices m_family_indices;
		VkQueue m_graphics_queue;

		const Instance* m_instance;
		const Window* m_window;

		void pickPhysicalDevice();
		static int rateDeviceSuitability(VkPhysicalDevice physical);
		static bool isDeviceSuitable(const VkPhysicalDevice& physical);
		static QueueFamilyIndices findQueueFamilies(const VkPhysicalDevice& physical);
	};
}