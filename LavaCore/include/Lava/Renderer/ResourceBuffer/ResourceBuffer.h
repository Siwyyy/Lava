#pragma once
#include "Lava/Application.h"

namespace Lava
{
	template <typename T>
	class ResourceBuffer
	{
	public:
		ResourceBuffer(const uint32_t size_ = 1)
			: m_size(size_) {}

		ResourceBuffer(const ResourceBuffer&) = default;
		virtual ~ResourceBuffer()             = default;

		virtual void updateMemory(const T* data_, uint32_t size_) = 0;
		virtual VkDescriptorBufferInfo getDescriptorBufferInfo() const = 0;

	protected:
		const std::shared_ptr<GraphicsContext>& m_context = Application::getInstance().getWindow().getContext();

		VkBuffer m_buffer;
		VkDeviceMemory m_memory;
		void* m_mapped;

		VkDeviceSize m_alignment;
		uint32_t m_size;
	};
}
