#pragma once
#include "ResourceBuffer.h"

#include "Lava/Renderer/Buffer.h"
#include "Lava/Renderer/Initializers.h"

namespace Lava
{
	template <typename T>
	class StorageBuffer final : public ResourceBuffer<T>
	{
	public:
		explicit StorageBuffer(const uint32_t capacity_)
		{
			this->m_alignment   = sizeof(T);
			this->m_memory_size = this->m_alignment * capacity_;

			Buffers::createVulkanBuffer(this->m_memory_size,
																	VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
																	VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
																	this->m_buffer,
																	this->m_memory);
			vkMapMemory(this->m_context->getDevice(), this->m_memory, 0, this->m_memory_size, NULL, &this->m_mapped);
		}

		StorageBuffer(const StorageBuffer&) = default;

		~StorageBuffer() override
		{
			vkDestroyBuffer(this->m_context->getDevice(), this->m_buffer, nullptr);
			vkFreeMemory(this->m_context->getDevice(), this->m_memory, nullptr);
		}

		inline void updateMemory(const T* data_, uint32_t size_ = 1) override
		{
			memcpy(this->m_mapped, data_, std::min(this->m_alignment * size_, this->m_memory_size));
		}

		inline VkDescriptorBufferInfo getDescriptorBufferInfo() const override { return Initializers::descriptorBufferInfo(this->m_buffer, 0, this->m_memory_size); }
		inline VkDescriptorType getDescriptorType() const override { return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER; }
	};
}
