#pragma once

#include "Gpu.hpp"

#include <vulkan/vulkan.h>

#include <vector>

namespace lvc
{
	class Gpu;

	class GpuManager
	{
	public:
		explicit GpuManager(const VkInstance& t_instance,
												const VkSurfaceKHR& t_surface);
		~GpuManager() = default;

		inline Gpu hGpu() const { return m_gpu; }

	private:
		// Members
		std::vector<Gpu> m_available_gpu;
		Gpu m_gpu;

		void selectGpu();
		void logAvailableGpu() const;
		void logSelectedGpu() const;
	};
}
