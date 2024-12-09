#pragma once
#include "Lava/Application.h"

namespace Lava
{
	template <typename T>
	class ResourceBuffer
	{
	public:
		ResourceBuffer()                      = default;
		ResourceBuffer(const ResourceBuffer&) = default;
		virtual ~ResourceBuffer()             = default;

		virtual void updateMemory(const T* data_, uint32_t size_) = 0;

		inline VkDeviceSize getAlignment() const { return this->m_alignment; }
		inline VkDeviceSize getMemorySize() const { return this->m_memory_size; };

		virtual VkDescriptorBufferInfo getDescriptorBufferInfo() const = 0;
		virtual VkDescriptorType getDescriptorType() const = 0;

	protected:
		const std::shared_ptr<GraphicsContext>& m_context = Application::getInstance().getWindow().getContext();

		VkBuffer m_buffer;
		VkDeviceMemory m_memory;
		void* m_mapped;

		VkDeviceSize m_memory_size;
		VkDeviceSize m_alignment;
	};
}
