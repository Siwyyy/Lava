#pragma once

#include "lvc/Instance.hpp"
#include "lvc/Window.hpp"

#include <optional>
#include <string>
#include <vector>
#include <vulkan/vulkan.h>

#include "DeviceExtensions.hpp"

namespace lvc
{
	class Device
	{
	private:
		struct QueueFamilyIndices
		{
			std::optional<uint32_t> graphics_family;
			std::optional<uint32_t> present_family;

			bool isComplete() const { return graphics_family.has_value() && present_family.has_value(); }
		};

		class PhysicalDevice
		{
		public:
			PhysicalDevice(const VkPhysicalDevice* physical, const Window& window);
			~PhysicalDevice();

			inline const uint32_t& score() const { return m_score; }
			inline const VkPhysicalDevice* physical() const { return m_physical; }
			inline const DeviceExtensions* extensions() const { return m_extensions; }
			inline const QueueFamilyIndices* indices() const { return m_indices; }

			void logInfo() const;

		private:
			const VkPhysicalDevice* m_physical;
			std::string m_name;
			DeviceExtensions* m_extensions;
			QueueFamilyIndices* m_indices;
			uint32_t m_score;

			void rateDeviceSuitability(const Window& window);
			void findQueueFamilies(VkSurfaceKHR surface);
		};

	public:
		Device(const Instance* instance, const Window* window);
		~Device();

		const VkPhysicalDevice* hPhysical() const { return m_physical; }
		VkDevice hLogical() const { return m_logical; }
		const QueueFamilyIndices* hFamilyIndices() const { return m_family_indices; }

	private:
		std::vector<PhysicalDevice*> m_physical_devices;

		const VkPhysicalDevice* m_physical;
		const DeviceExtensions* m_device_extensions;
		const QueueFamilyIndices* m_family_indices;

		VkDevice m_logical;

		VkQueue m_graphics_queue;
		VkQueue m_present_queue;

		const Instance* m_instance;
		const Window* m_window;

		void pickPhysicalDevice();
	};
}
