#include "Application.hpp"

#include "CommandBuffer.hpp"
#include "CommandPool.hpp"
#include "DebugMessenger.hpp"
#include "Device.hpp"
#include "Gpu.hpp"
#include "GpuManager.hpp"
#include "GraphicsPipeline.hpp"
#include "Instance.hpp"
#include "RenderPass.hpp"
#include "Swapchain.hpp"
#include "Window.hpp"

#include <iostream>

using namespace lvc;

constexpr int WIDTH  = 800;
constexpr int HEIGHT = 600;

Application::Application()
	: m_instance("LavaCore", "No Engine")
	, m_debug_messenger(m_instance.hVkInstance())
	, m_window(WIDTH, HEIGHT, "LavaCore - Test", m_instance.hVkInstance())
	, m_gpu_manager(m_instance.hVkInstance(), m_window.hVkSurface())
	, m_gpu(m_gpu_manager.hGpu())
	, m_device(m_gpu.hVkPhysicalDevice(), m_gpu.hIndices())
	, m_swapchain(m_device.hVkDevice(), m_gpu.hVkPhysicalDevice(), m_window.hGlfwWindow(), m_window.hVkSurface(), m_gpu.hIndices())
	, m_render_pass(m_device.hVkDevice(), m_swapchain.hExtent2d(), m_swapchain.hFormat(), m_swapchain.hImageViews())
	, m_graphics_pipeline(m_device.hVkDevice(), m_swapchain.hExtent2d(), m_render_pass.hRenderPass(), m_render_pass)
	, m_command_pool(m_device.hVkDevice(), m_gpu.hIndices())
	, m_command_buffer(m_command_pool.hCommandPool(), m_device.hVkDevice(), m_render_pass.hRenderPass(), m_render_pass.hFramebuffers(), m_graphics_pipeline.hPipeline(), m_swapchain.hExtent2d())
	, m_semaphore_image_available(nullptr)
	, m_semaphore_render_finished(nullptr)
	, m_fence_in_flight(nullptr) {}

Application::~Application()
{
	vkDestroySemaphore(m_device.hVkDevice(), m_semaphore_image_available, nullptr);
	vkDestroySemaphore(m_device.hVkDevice(), m_semaphore_render_finished, nullptr);
	vkDestroyFence(m_device.hVkDevice(), m_fence_in_flight, nullptr);
}

void Application::run()
{
	createSyncObjects(m_device.hVkDevice());
	mainLoop();
}

void Application::mainLoop() const
{
	while (!glfwWindowShouldClose(&m_window.hGlfwWindow()))
	{
		glfwPollEvents();
		draw(m_device.hVkDevice(), m_command_buffer.hCommandBuffer(), m_device.hGraphicsQueue(), m_device.hPresentQueue(), m_swapchain.hSwapchain());
	}
}

void Application::createSyncObjects(const VkDevice& t_device)
{
	VkSemaphoreCreateInfo semaphore_create_info;
	semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	semaphore_create_info.flags = 0;
	semaphore_create_info.pNext = nullptr;

	VkFenceCreateInfo fence_create_info;
	fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
	fence_create_info.pNext = nullptr;

	if (vkCreateSemaphore(t_device, &semaphore_create_info, nullptr, &m_semaphore_image_available) != VK_SUCCESS ||
			vkCreateSemaphore(t_device, &semaphore_create_info, nullptr, &m_semaphore_render_finished) != VK_SUCCESS ||
			vkCreateFence(t_device, &fence_create_info, nullptr, &m_fence_in_flight) != VK_SUCCESS)
		throw std::runtime_error("err: Failed to create semaphores/fence!\n");
}

void Application::draw(const VkDevice& t_device,
											 const VkCommandBuffer& t_command_buffer,
											 const VkQueue& t_graphics_queue,
											 const VkQueue& t_present_queue,
											 const VkSwapchainKHR& t_swapchain) const
{
	vkWaitForFences(t_device, 1, &m_fence_in_flight,VK_TRUE,UINT64_MAX);
	vkResetFences(t_device, 1, &m_fence_in_flight);

	uint32_t image_index;
	vkAcquireNextImageKHR(m_device.hVkDevice(), m_swapchain.hSwapchain(), UINT64_MAX, m_semaphore_image_available, VK_NULL_HANDLE, &image_index);

	vkResetCommandBuffer(m_command_buffer.hCommandBuffer(), 0);
	m_command_buffer.recordCommandBuffer(image_index);

	const VkSemaphore wait_semaphores[]      = {m_semaphore_image_available};
	const VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	const VkSemaphore signal_semaphores[]    = {m_semaphore_render_finished};

	VkSubmitInfo submit_info;
	submit_info.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.waitSemaphoreCount   = 1;
	submit_info.pWaitSemaphores      = wait_semaphores;
	submit_info.pWaitDstStageMask    = wait_stages;
	submit_info.commandBufferCount   = 1;
	submit_info.pCommandBuffers      = &t_command_buffer;
	submit_info.signalSemaphoreCount = 1;
	submit_info.pSignalSemaphores    = signal_semaphores;
	submit_info.pNext                = nullptr;

	if (vkQueueSubmit(t_graphics_queue, 1, &submit_info, m_fence_in_flight) != VK_SUCCESS)
		throw std::runtime_error("err: Failed to submit draw command buffer!\n");

	const VkSwapchainKHR swapchains[] = {t_swapchain};

	VkPresentInfoKHR present_info;
	present_info.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	present_info.waitSemaphoreCount = 1;
	present_info.pWaitSemaphores    = signal_semaphores;
	present_info.swapchainCount     = 1;
	present_info.pSwapchains        = swapchains;
	present_info.pImageIndices      = &image_index;
	present_info.pResults           = nullptr;
	present_info.pNext              = nullptr;

	vkQueuePresentKHR(t_present_queue, &present_info);
}
