#pragma once

#include "ResourceBuffer.h"

#include "Lava/Renderer/Buffer.h"
#include "Lava/Renderer/Initializers.h"

namespace Lava
{
	template <typename T>
	class UniformBuffer final : public ResourceBuffer<T>
	{
	public:
		UniformBuffer()
			: ResourceBuffer<T>()
		{
			VkDeviceSize static_buffer_size = sizeof(T);
			Buffers::createVulkanBuffer(static_buffer_size,
																	VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
																	VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
																	this->m_buffer,
																	this->m_memory);
			vkMapMemory(this->m_context->getDevice(), this->m_memory, 0, static_buffer_size,NULL, &this->m_mapped);
		}

		UniformBuffer(const UniformBuffer&) = default;

		~UniformBuffer() override
		{
			vkDestroyBuffer(this->m_context->getDevice(), this->m_buffer, nullptr);
			vkFreeMemory(this->m_context->getDevice(), this->m_memory, nullptr);
		}

		inline void updateMemory(const T* data_, uint32_t size_ = 1) override
		{
			memcpy(this->m_mapped, data_, sizeof(T));
		}

		inline VkDescriptorBufferInfo getDescriptorBufferInfo() const override
		{
			return Initializers::descriptorBufferInfo(this->m_buffer, 0, sizeof(T));
		}
	};
}
