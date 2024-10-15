#include "Lavapch.h"
#include "Application.h"

#include "Log.h"

#include "Events/MouseEvent.h"

using namespace Lava;

Application::Application()
	: m_debug_messenger(m_instance.hVkInstance())
	, m_window(WINDOW_WIDTH,
						 WINDOW_HEIGHT,
						 "LavaCore - Test",
						 m_instance.hVkInstance(),
						 this)
	, m_gpu_manager(m_instance.hVkInstance(),
									m_window.hVkSurface())
	, m_gpu(m_gpu_manager.hGpu())
	, m_device(m_gpu.hVkPhysicalDevice(),
						 m_gpu.hIndices())
	, m_swapchain(m_device.hVkDevice(),
								m_gpu.hVkPhysicalDevice(),
								m_window.hGlfwWindow(),
								m_window.hVkSurface(),
								m_gpu.hIndices())
	, m_render_pass(m_device.hVkDevice(),
									m_swapchain.hExtent2d(),
									m_swapchain.hFormat(),
									m_swapchain.hImageViews())
	, m_graphics_pipeline(m_device.hVkDevice(),
												m_swapchain.hExtent2d(),
												m_render_pass.hRenderPass(),
												m_render_pass)
	, m_command_pool(m_device.hVkDevice(),
									 m_gpu.hIndices())
	, m_vertex_buffer(m_device.hVkDevice(),
										m_gpu.hVkPhysicalDevice())
	, m_command_buffer(m_command_pool.hCommandPool(),
										 m_device.hVkDevice(),
										 m_render_pass.hRenderPass(),
										 m_render_pass.hFramebuffers(),
										 m_graphics_pipeline.hPipeline(),
										 m_swapchain.hExtent2d(),
										 MAX_FRAMES_IN_FLIGHT,
										 m_vertex_buffer.hVertexBuffer())
	, m_sync_objects(m_device.hVkDevice(),
									 MAX_FRAMES_IN_FLIGHT) {}

void Application::run()
{
	MouseMovedEvent e(1.243f, 32.512f);
	LAVA_CORE_INFO(e);
	mainLoop();
}

void Application::mainLoop()
{
	while (!glfwWindowShouldClose(&m_window.hGlfwWindow()))
	{
		glfwPollEvents();
		draw(m_device.hVkDevice(),
				 m_command_buffer.hCommandBuffers(),
				 m_device.hGraphicsQueue(),
				 m_device.hPresentQueue(),
				 m_swapchain.hSwapchain());
	}
	vkDeviceWaitIdle(m_device.hVkDevice());
}

void Application::draw(const VkDevice& t_device,
											 const std::vector<VkCommandBuffer>& t_command_buffers,
											 const VkQueue& t_graphics_queue,
											 const VkQueue& t_present_queue,
											 const VkSwapchainKHR& t_swapchain)
{
	vkWaitForFences(t_device, 1, &m_sync_objects.hFenceInFlight(m_current_frame),VK_TRUE,UINT64_MAX);

	uint32_t image_index;
	VkResult result = vkAcquireNextImageKHR(m_device.hVkDevice(),
																					m_swapchain.hSwapchain(),
																					UINT64_MAX,
																					m_sync_objects.hSemaphoreImageAvailable(m_current_frame),
																					VK_NULL_HANDLE,
																					&image_index);

	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		recreateSwapchain();
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
	{
		LAVA_CORE_ERROR("Failed to acquire swapchain image!");
	}

	vkResetFences(t_device, 1, &m_sync_objects.hFenceInFlight(m_current_frame));
	vkResetCommandBuffer(t_command_buffers[m_current_frame], 0);
	m_command_buffer.recordCommandBuffer(m_current_frame, image_index);

	const VkSemaphore wait_semaphores[]      = {m_sync_objects.hSemaphoreImageAvailable(m_current_frame)};
	const VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	const VkSemaphore signal_semaphores[]    = {m_sync_objects.hSemaphoreRenderFinished(m_current_frame)};

	VkSubmitInfo submit_info;
	submit_info.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.waitSemaphoreCount   = 1;
	submit_info.pWaitSemaphores      = wait_semaphores;
	submit_info.pWaitDstStageMask    = wait_stages;
	submit_info.commandBufferCount   = 1;
	submit_info.pCommandBuffers      = &t_command_buffers[m_current_frame];
	submit_info.signalSemaphoreCount = 1;
	submit_info.pSignalSemaphores    = signal_semaphores;
	submit_info.pNext                = nullptr;

	if (vkQueueSubmit(t_graphics_queue, 1, &submit_info, m_sync_objects.hFenceInFlight(m_current_frame)) != VK_SUCCESS)
		LAVA_CORE_ERROR("Failed to submit draw command buffer!");

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

	result = vkQueuePresentKHR(t_present_queue, &present_info);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || frame_buffer_resized)
	{
		frame_buffer_resized = false;
		recreateSwapchain();
		return;
	}
	else if (result != VK_SUCCESS)
	{
		LAVA_CORE_ERROR("Failed to acquire swapchain image!");
	}

	m_current_frame = (m_current_frame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Application::recreateSwapchain()
{
	int width = 0, height = 0;
	glfwGetFramebufferSize(&m_window.hGlfwWindow(), &width, &height);
	while (width == 0 || height == 0)
	{
		glfwGetFramebufferSize(&m_window.hGlfwWindow(), &width, &height);
		glfwWaitEvents();
	}

	vkDeviceWaitIdle(m_device.hVkDevice());

	m_swapchain.recreate();
	m_render_pass.recreateFrameBuffers();
}
