#include "Lavapch.h"
#include "VulkanWindow.h"

#include "Log/Log.h"

#include "Events/ApplicationEvent.h"
#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"

#include "Vertex.h"

#define GLM_FORCE_RADIANS
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#include <chrono>

using namespace Lava;

#ifdef NDEBUG
const bool VulkanWindow::validation_layers_enabled = false;
#else
const bool VulkanWindow::validation_layers_enabled = true;
#endif

namespace
{
	bool l_glfw_initialized = false;
}

Window* Window::create(const WindowProps& props_)
{
	return new VulkanWindow(props_);
}

VulkanWindow::VulkanWindow(const WindowProps& props_)
{
	init(props_);
}

VulkanWindow::~VulkanWindow()
{
	shutdown();
}

void VulkanWindow::init(const WindowProps& props_)
{
	m_data.title  = props_.title;
	m_data.width  = props_.width;
	m_data.height = props_.height;

	createGlfwWindow();

	LAVA_CORE_INFO("Initializing Vulkan graphics...");

	createVulkanInstance();
	createVulkanDebug();
	createVulkanDevice();
	createVulkanSwapchain();
	createVulkanRenderPass();

	createVulkanDescriptorSetLayout();
	createVulkanGraphicsPipeline();
	createVulkanFrameBuffers();

	createVulkanCommandPool();
	allocateVulkanCommandBuffers();

	createVulkanVertexBuffer();
	createVulkanIndexBuffer();
	createVulkanUniformBuffers();

	createVulkanDescriptorPool();
	createVulkanDescriptorSets();

	createVulkanSyncObjects();

	LAVA_CORE_INFO("Vulkan graphics initialization complete");
}

void VulkanWindow::shutdown()
{
	for (size_t i = 0; i < m_data.max_frames_in_flight; i++)
	{
		vkDestroySemaphore(m_device, m_semaphore_image_available[i], nullptr);
		vkDestroySemaphore(m_device, m_semaphore_render_finished[i], nullptr);
		vkDestroyFence(m_device, m_fence_in_flight[i], nullptr);
	}

	vkDestroyBuffer(m_device, m_index_buffer, nullptr);
	vkFreeMemory(m_device, m_index_buffer_memory, nullptr);
	vkDestroyBuffer(m_device, m_vertex_buffer, nullptr);
	vkFreeMemory(m_device, m_vertex_buffer_memory, nullptr);

	vkDestroyCommandPool(m_device, m_command_pool, nullptr);

	vkDestroyPipeline(m_device, m_pipeline, nullptr);
	vkDestroyPipelineLayout(m_device, m_pipeline_layout, nullptr);
	vkDestroyDescriptorPool(m_device, m_descriptor_pool, nullptr);
	vkDestroyDescriptorSetLayout(m_device, m_descriptor_set_layout, nullptr);
	for (size_t i = 0; i < m_data.max_frames_in_flight; i++)
	{
		vkDestroyBuffer(m_device, m_uniform_buffers[i], nullptr);
		vkFreeMemory(m_device, m_uniform_buffers_memory[i], nullptr);
	}
	vkDestroyShaderModule(m_device, m_vert_shader_module, nullptr);
	vkDestroyShaderModule(m_device, m_frag_shader_module, nullptr);

	destroyVulkanFrameBuffers();
	vkDestroyRenderPass(m_device, m_render_pass, nullptr);

	for (const VkImageView& image_view : m_image_views)
		vkDestroyImageView(m_device, image_view, nullptr);
	vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);

	vkDestroyDevice(m_device, nullptr);

	destroyVulkanDebug();

	vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
	vkDestroyInstance(m_instance, nullptr);

	glfwDestroyWindow(m_window);
	glfwTerminate();
}

void VulkanWindow::onUpdate()
{
	glfwPollEvents();
	draw();
	vkDeviceWaitIdle(m_device);
}

void VulkanWindow::onMouseMoved(uint32_t angle_) { m_angle = angle_; }

void VulkanWindow::draw()
{
	vkWaitForFences(m_device, 1, &m_fence_in_flight[m_current_frame],VK_TRUE,UINT64_MAX);

	uint32_t image_index;
	VkResult result = vkAcquireNextImageKHR(m_device,
																					m_swapchain,
																					UINT64_MAX,
																					m_semaphore_image_available[m_current_frame],
																					VK_NULL_HANDLE,
																					&image_index);

	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		recreateVulkanSwapchain();
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
	{
		LAVA_CORE_ERROR("Failed to acquire swapchain image!");
	}

	vkResetFences(m_device, 1, &m_fence_in_flight[m_current_frame]);
	vkResetCommandBuffer(m_command_buffers[m_current_frame], 0);
	recordVulkanCommandBuffer(m_current_frame, image_index);

	const VkSemaphore wait_semaphores[]      = {m_semaphore_image_available[m_current_frame]};
	const VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	const VkSemaphore signal_semaphores[]    = {m_semaphore_render_finished[m_current_frame]};

	updateUniformBuffer(m_current_frame);

	VkSubmitInfo submit_info;
	submit_info.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.waitSemaphoreCount   = 1;
	submit_info.pWaitSemaphores      = wait_semaphores;
	submit_info.pWaitDstStageMask    = wait_stages;
	submit_info.commandBufferCount   = 1;
	submit_info.pCommandBuffers      = &m_command_buffers[m_current_frame];
	submit_info.signalSemaphoreCount = 1;
	submit_info.pSignalSemaphores    = signal_semaphores;
	submit_info.pNext                = nullptr;

	if (vkQueueSubmit(m_graphics_queue, 1, &submit_info, m_fence_in_flight[m_current_frame]) != VK_SUCCESS)
		LAVA_CORE_ERROR("Failed to submit draw command buffer!");

	const VkSwapchainKHR swapchains[] = {m_swapchain};

	VkPresentInfoKHR present_info;
	present_info.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	present_info.waitSemaphoreCount = 1;
	present_info.pWaitSemaphores    = signal_semaphores;
	present_info.swapchainCount     = 1;
	present_info.pSwapchains        = swapchains;
	present_info.pImageIndices      = &image_index;
	present_info.pResults           = nullptr;
	present_info.pNext              = nullptr;

	result = vkQueuePresentKHR(m_present_queue, &present_info);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || frame_buffer_resized)
	{
		recreateVulkanSwapchain();
		frame_buffer_resized = false;
		return;
	}
	else if (result != VK_SUCCESS)
	{
		LAVA_CORE_ERROR("Failed to acquire swapchain image!");
	}

	m_current_frame = (m_current_frame + 1) % m_data.max_frames_in_flight;
}

// ////////////////////////// //
// Initial creation functions //
// ////////////////////////// //

void VulkanWindow::createGlfwWindow()
{
	LAVA_CORE_INFO("Creating window \"{0}\"...", m_data.title);

	if (!l_glfw_initialized)
	{
		glfwInit();
		l_glfw_initialized = true;
	}

	// Create GLFW Window ///////////////////////////////////
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	m_window = glfwCreateWindow((int)m_data.width, (int)m_data.height, m_data.title.c_str(), nullptr, nullptr);
	glfwSetWindowUserPointer(m_window, &m_data);

	// Set callbacks ////////////////////////////////////////
	glfwSetWindowSizeCallback(m_window,
														[](GLFWwindow* window_, int width_, int height_)
														{
															WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window_);
															data.width       = width_;
															data.height      = height_;
															WindowResizeEvent event(width_, height_);
															data.EventCallback(event);
														});

	glfwSetWindowCloseCallback(m_window,
														 [](GLFWwindow* window_)
														 {
															 WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window_);
															 WindowCloseEvent event;
															 data.EventCallback(event);
														 });

	glfwSetKeyCallback(m_window,
										 [](GLFWwindow* window_, int key_, int scancode_, int action_, int mods_)
										 {
											 WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window_);

											 switch (action_)
											 {
											 case GLFW_PRESS:
												 {
													 KeyPressedEvent event(key_, false);
													 data.EventCallback(event);
													 break;
												 }
											 case GLFW_RELEASE:
												 {
													 KeyReleasedEvent event(key_);
													 data.EventCallback(event);
													 break;
												 }
											 case GLFW_REPEAT:
												 {
													 KeyPressedEvent event(key_, true);
													 data.EventCallback(event);
													 break;
												 }
											 }
										 });

	glfwSetMouseButtonCallback(m_window,
														 [](GLFWwindow* window_, int button_, int action_, int mods_)
														 {
															 WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window_);

															 switch (action_)
															 {
															 case GLFW_PRESS:
																 {
																	 MouseButtonPressedEvent event(button_);
																	 data.EventCallback(event);
																	 break;
																 }
															 case GLFW_RELEASE:
																 {
																	 MouseButtonReleasedEvent event(button_);
																	 data.EventCallback(event);
																	 break;
																 }
															 }
														 });

	glfwSetCursorPosCallback(m_window,
													 [](GLFWwindow* window_, double x_, double y_)
													 {
														 WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window_);
														 MouseMovedEvent event((float)x_, (float)y_);
														 data.EventCallback(event);
													 });

	LAVA_CORE_INFO("Window \"{0}\" created", m_data.title);
}

void VulkanWindow::createVulkanInstance()
{
	// Query VkInstance extensions //////////////////////////
	uint32_t extension_count;
	vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);
	std::vector<VkExtensionProperties> extensions(extension_count);
	vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extensions.data());
	for (const auto& extension : extensions)
		m_available_instance_ext.emplace_back(extension.extensionName);

	uint32_t glfw_extension_count  = 0;
	const char** glfw_extensions_p = glfwGetRequiredInstanceExtensions(&glfw_extension_count);
	m_required_instance_ext.append_range(std::vector<const char*>{glfw_extensions_p,glfw_extensions_p + glfw_extension_count});

	if (validation_layers_enabled)
		m_required_instance_ext.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

	// Check VkInstance extensions //////////////////////////
	LAVA_CORE_DEBUG("Checking instance extensions...");
	uint32_t available_extension_count = 0;
	for (const auto& required : m_required_instance_ext)
	{
		bool found = false;
		for (const auto& available : m_available_instance_ext)
			if (!strcmp(required, available))
			{
				found = true;
				available_extension_count++;
				LAVA_CORE_DEBUG("(Available) {0}", required);
				break;
			}
		if (!found)
			LAVA_CORE_ERROR("(Missing) {0}", required);
	}
	if (available_extension_count == m_required_instance_ext.size())
		LAVA_CORE_DEBUG("Instance extensions - all available");

	// Check validation layers support //////////////////////
	if (validation_layers_enabled && !checkValidationLayerSupport())
	{
		LAVA_CORE_ERROR("Validation layers requested, but not available!");
		LAVA_DEBUGBREAK
	}

	// Create VkInstance ////////////////////////////////////
	VkInstanceCreateInfo instance_create_info;
	instance_create_info.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instance_create_info.pNext                   = nullptr;
	instance_create_info.flags                   = NULL;
	instance_create_info.pApplicationInfo        = nullptr;
	instance_create_info.enabledLayerCount       = 0;
	instance_create_info.ppEnabledLayerNames     = nullptr;
	instance_create_info.enabledExtensionCount   = static_cast<uint32_t>(m_required_instance_ext.size());
	instance_create_info.ppEnabledExtensionNames = m_required_instance_ext.data();

	VkDebugUtilsMessengerCreateInfoEXT debug_create_info;
	if (validation_layers_enabled)
	{
		instance_create_info.enabledLayerCount   = static_cast<uint32_t>(validation_layers.size());
		instance_create_info.ppEnabledLayerNames = validation_layers.data();
		debug_create_info                        = debugCreateInfo();
		instance_create_info.pNext               = &debug_create_info;
	}

	if (vkCreateInstance(&instance_create_info, nullptr, &m_instance) != VK_SUCCESS)
		LAVA_CORE_ERROR("Failed to create instance!");

	LAVA_CORE_DEBUG("Created: VkInstance");
}

void VulkanWindow::createVulkanDebug()
{
	if (!validation_layers_enabled)
		return;

	const auto create_func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
		vkGetInstanceProcAddr(m_instance, "vkCreateDebugUtilsMessengerEXT"));

	LAVA_ASSERT(create_func, "Failed to get function vkCreateDebugUtilsMessengerEXT!")

	VkDebugUtilsMessengerCreateInfoEXT debug_info = debugCreateInfo();

	if (create_func(m_instance, &debug_info, nullptr, &m_debug) != VK_SUCCESS)
		LAVA_CORE_ERROR("Failed to create debug utils messenger");

	LAVA_CORE_DEBUG("Created: VkDebugUtilsMessenger");
}

void VulkanWindow::createVulkanDevice()
{
	LAVA_CORE_DEBUG("Searching for GPUs...");

	// Enumerate GPUs ///////////////////////////////////////
	uint32_t available_gpus_count = 0;
	vkEnumeratePhysicalDevices(m_instance, &available_gpus_count, nullptr);
	LAVA_ASSERT(available_gpus_count, "Failed to find GPUs with Vulkan support!")
	std::vector<VkPhysicalDevice> available_gpus(available_gpus_count);
	vkEnumeratePhysicalDevices(m_instance, &available_gpus_count, available_gpus.data());

	m_gpu                       = available_gpus[0];
	uint32_t selected_gpu_score = 0;
	for (const auto& gpu : available_gpus)
	{
		uint32_t gpu_score = 0;

		// Check gpu extensions support ///////////////////////////
		uint32_t ext_count;
		vkEnumerateDeviceExtensionProperties(gpu, nullptr, &ext_count, nullptr);
		std::vector<VkExtensionProperties> available_gpu_ext(ext_count);
		vkEnumerateDeviceExtensionProperties(gpu, nullptr, &ext_count, available_gpu_ext.data());

		uint32_t ext_not_found = (uint32_t)m_required_gpu_ext.size();
		for (const auto& required : m_required_gpu_ext)
			for (const auto& available : available_gpu_ext)
				if (!strcmp(required, available.extensionName))
				{
					ext_not_found--;
				}

		if (ext_not_found)
			gpu_score = 0;

		// Check device features support //////////////////////
		VkPhysicalDeviceFeatures gpu_features;
		vkGetPhysicalDeviceFeatures(gpu, &gpu_features);
		if (!gpu_features.geometryShader)
			gpu_score = 0;

		// Check device properties //////////////////////
		VkPhysicalDeviceProperties gpu_props;
		vkGetPhysicalDeviceProperties(gpu, &gpu_props);
		if (gpu_props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
			gpu_score += 1000;
		gpu_score += static_cast<int>(gpu_props.limits.maxImageDimension2D);

		if (gpu_score > selected_gpu_score)
		{
			m_gpu              = gpu;
			selected_gpu_score = gpu_score;
		}
		LAVA_CORE_DEBUG(" - {0}", gpu_props.deviceName);
	}
	LAVA_ASSERT(selected_gpu_score, "Failed to create suitable GPU")

	VkPhysicalDeviceProperties gpu_props;
	vkGetPhysicalDeviceProperties(m_gpu, &gpu_props);
	LAVA_CORE_INFO("Selected GPU: {0}", gpu_props.deviceName);

	// get VkSurface from GLFW //////////////////////////////
	if (glfwCreateWindowSurface(m_instance, m_window, nullptr, &m_surface) != VK_SUCCESS)
		LAVA_CORE_ERROR("Unable to create window surface!");

	// Get queue families
	uint32_t family_count;
	vkGetPhysicalDeviceQueueFamilyProperties(m_gpu, &family_count, nullptr);
	std::vector<VkQueueFamilyProperties> families(family_count);
	vkGetPhysicalDeviceQueueFamilyProperties(m_gpu, &family_count, families.data());

	// Iterate through families until one that supports requirements is found

	bool found = false;
	for (int i = 0; !found && i < families.size(); i++)
	{
		const auto& family = families[i];

		//  Check for graphics support
		if (family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			m_queue_family_indices.graphics = i;

		// Check for VkSurface presentation support
		VkBool32 present_support = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(m_gpu, i, m_surface, &present_support);
		if (present_support)
			m_queue_family_indices.present = i;

		found = m_queue_family_indices.isComplete();
	}

	// Setup queue families
	const std::set<uint32_t> unique_queue_families = {m_queue_family_indices.graphics.value(),
																										m_queue_family_indices.present.value()};

	std::vector<VkDeviceQueueCreateInfo> queue_infos;
	constexpr float priority = 1.0f;
	for (const uint32_t& queue_family : unique_queue_families)
	{
		VkDeviceQueueCreateInfo queue_create_info;
		queue_create_info.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queue_create_info.pNext            = nullptr;
		queue_create_info.flags            = NULL;
		queue_create_info.queueFamilyIndex = queue_family;
		queue_create_info.queueCount       = 1;
		queue_create_info.pQueuePriorities = &priority;
		queue_infos.push_back(queue_create_info);
	}

	// Setup logical device
	VkDeviceCreateInfo device_create_info;
	device_create_info.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	device_create_info.pNext                   = nullptr;
	device_create_info.flags                   = NULL;
	device_create_info.queueCreateInfoCount    = static_cast<uint32_t>(queue_infos.size());
	device_create_info.pQueueCreateInfos       = queue_infos.data();
	device_create_info.enabledLayerCount       = 0;       // optional
	device_create_info.ppEnabledLayerNames     = nullptr; // optional
	device_create_info.enabledExtensionCount   = static_cast<uint32_t>(m_required_gpu_ext.size());
	device_create_info.ppEnabledExtensionNames = m_required_gpu_ext.data();
	device_create_info.pEnabledFeatures        = nullptr;

	if (validation_layers_enabled)
	{
		device_create_info.enabledLayerCount   = static_cast<uint32_t>(validation_layers.size());
		device_create_info.ppEnabledLayerNames = validation_layers.data();
	}

	if (vkCreateDevice(m_gpu, &device_create_info, nullptr, &m_device) != VK_SUCCESS)
		LAVA_CORE_ERROR("Failed to create logical device!");

	LAVA_CORE_DEBUG("Created: VkDevice");

	// Get handles for graphics and presentation queues
	vkGetDeviceQueue(m_device, m_queue_family_indices.graphics.value(), 0, &m_graphics_queue);
	vkGetDeviceQueue(m_device, m_queue_family_indices.present.value(), 0, &m_present_queue);
}

void VulkanWindow::createVulkanSwapchain()
{
	// Query Swapchain support details //////////////////////
	SwapchainSupportDetails details;
	// Extent
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_gpu, m_surface, &details.surface_capabilities);

	// Surface format
	uint32_t format_count;
	vkGetPhysicalDeviceSurfaceFormatsKHR(m_gpu, m_surface, &format_count, nullptr);
	details.surface_formats.resize(format_count);
	vkGetPhysicalDeviceSurfaceFormatsKHR(m_gpu, m_surface, &format_count, details.surface_formats.data());

	// Surface present mode
	uint32_t mode_count;
	vkGetPhysicalDeviceSurfacePresentModesKHR(m_gpu, m_surface, &mode_count, nullptr);
	details.present_modes.resize(mode_count);
	vkGetPhysicalDeviceSurfacePresentModesKHR(m_gpu, m_surface, &mode_count, details.present_modes.data());

	if (details.surface_formats.empty() || details.present_modes.empty())
		LAVA_CORE_ERROR("Device details not supported! SWAPCHAIN cannot be created!");

	// Set VkExtent2D ///////////////////////////////////////
	m_extent_2d = details.surface_capabilities.currentExtent;
	if (details.surface_capabilities.currentExtent.width == std::numeric_limits<uint32_t>::max())
	{
		int width_int, height_int;
		glfwGetFramebufferSize(m_window, &width_int, &height_int);

		const uint32_t width = std::clamp(static_cast<uint32_t>(width_int),
																			details.surface_capabilities.minImageExtent.width,
																			details.surface_capabilities.maxImageExtent.width);
		const uint32_t height = std::clamp(static_cast<uint32_t>(height_int),
																			 details.surface_capabilities.minImageExtent.height,
																			 details.surface_capabilities.maxImageExtent.height);

		m_extent_2d = {width,height};
	}

	{ // Set VkSurfaceFormat /////////////////////////////////
		bool found = false;
		for (const auto& format : details.surface_formats)
			if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			{
				m_surface_format = format;
				found            = true;
			}
		if (!found)
			m_surface_format = details.surface_formats[0];
	}

	{ // Set VkPresentMode //////////////////////////////////
		bool found = false;
		for (const auto& mode : details.present_modes)
			if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				m_present_mode = mode;
				found          = true;
			}
		if (!found)
			m_present_mode = VK_PRESENT_MODE_FIFO_KHR;
	}

	// Swapchain creation ///////////////////////////////////
	uint32_t image_count = details.surface_capabilities.minImageCount + 1;
	if (details.surface_capabilities.maxImageCount > 0 && image_count < details.surface_capabilities.maxImageCount)
		image_count = details.surface_capabilities.maxImageCount;

	VkSwapchainCreateInfoKHR swapchain_create_info;
	swapchain_create_info.sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchain_create_info.pNext            = nullptr;
	swapchain_create_info.flags            = NULL;
	swapchain_create_info.surface          = m_surface;
	swapchain_create_info.minImageCount    = image_count;
	swapchain_create_info.imageFormat      = m_surface_format.format;
	swapchain_create_info.imageColorSpace  = m_surface_format.colorSpace;
	swapchain_create_info.imageExtent      = m_extent_2d;
	swapchain_create_info.imageArrayLayers = 1;
	swapchain_create_info.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	const uint32_t queue_family_indices[] = {m_queue_family_indices.graphics.value(),m_queue_family_indices.present.value()};
	if (m_queue_family_indices.graphics.value() != m_queue_family_indices.present.value())
	{
		swapchain_create_info.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
		swapchain_create_info.queueFamilyIndexCount = 2;
		swapchain_create_info.pQueueFamilyIndices   = queue_family_indices;
	}
	else
	{
		swapchain_create_info.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
		swapchain_create_info.queueFamilyIndexCount = 0;
		swapchain_create_info.pQueueFamilyIndices   = nullptr;
	}

	swapchain_create_info.preTransform   = details.surface_capabilities.currentTransform;
	swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapchain_create_info.presentMode    = m_present_mode;
	swapchain_create_info.clipped        = VK_TRUE;
	swapchain_create_info.oldSwapchain   = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(m_device, &swapchain_create_info, nullptr, &m_swapchain) != VK_SUCCESS)
		LAVA_CORE_ERROR("Failed to create swapchain!");

	LAVA_CORE_DEBUG("Created: VkSwapchainKHR");

	vkGetSwapchainImagesKHR(m_device, m_swapchain, &image_count, nullptr);
	m_images.resize(image_count);
	vkGetSwapchainImagesKHR(m_device, m_swapchain, &image_count, m_images.data());

	m_image_views.resize(m_images.size());
	for (size_t i = 0; i < m_image_views.size(); i++)
	{
		VkImageViewCreateInfo image_view_create_info;
		image_view_create_info.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		image_view_create_info.pNext                           = nullptr;
		image_view_create_info.flags                           = NULL;
		image_view_create_info.image                           = m_images[i];
		image_view_create_info.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
		image_view_create_info.format                          = m_surface_format.format;
		image_view_create_info.components.r                    = VK_COMPONENT_SWIZZLE_IDENTITY;
		image_view_create_info.components.g                    = VK_COMPONENT_SWIZZLE_IDENTITY;
		image_view_create_info.components.b                    = VK_COMPONENT_SWIZZLE_IDENTITY;
		image_view_create_info.components.a                    = VK_COMPONENT_SWIZZLE_IDENTITY;
		image_view_create_info.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
		image_view_create_info.subresourceRange.baseMipLevel   = 0;
		image_view_create_info.subresourceRange.levelCount     = 1;
		image_view_create_info.subresourceRange.baseArrayLayer = 0;
		image_view_create_info.subresourceRange.layerCount     = 1;

		if (vkCreateImageView(m_device, &image_view_create_info, nullptr, &m_image_views[i]) != VK_SUCCESS)
		{
			LAVA_CORE_ERROR("failed to create image views!");
		}
	}
	LAVA_CORE_DEBUG("Created: VkImageViews");
}

void VulkanWindow::createVulkanRenderPass()
{
	VkAttachmentDescription attachment_description;
	attachment_description.flags          = 0;
	attachment_description.format         = m_surface_format.format;
	attachment_description.samples        = VK_SAMPLE_COUNT_1_BIT;
	attachment_description.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachment_description.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
	attachment_description.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachment_description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachment_description.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
	attachment_description.finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference attachment_reference;
	attachment_reference.attachment = 0;
	attachment_reference.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass_description;
	subpass_description.flags                   = 0;
	subpass_description.pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass_description.inputAttachmentCount    = 0;
	subpass_description.pInputAttachments       = nullptr;
	subpass_description.colorAttachmentCount    = 1;
	subpass_description.pColorAttachments       = &attachment_reference;
	subpass_description.pResolveAttachments     = VK_NULL_HANDLE;
	subpass_description.pDepthStencilAttachment = VK_NULL_HANDLE;
	subpass_description.preserveAttachmentCount = 0;
	subpass_description.pPreserveAttachments    = nullptr;

	VkSubpassDependency subpass_dependency;
	subpass_dependency.srcSubpass      = VK_SUBPASS_EXTERNAL;
	subpass_dependency.dstSubpass      = 0;
	subpass_dependency.srcStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subpass_dependency.dstStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subpass_dependency.srcAccessMask   = 0;
	subpass_dependency.dstAccessMask   = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	subpass_dependency.dependencyFlags = NULL;

	VkRenderPassCreateInfo render_pass_info;
	render_pass_info.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	render_pass_info.pNext           = nullptr;
	render_pass_info.flags           = NULL;
	render_pass_info.attachmentCount = 1;
	render_pass_info.pAttachments    = &attachment_description;
	render_pass_info.subpassCount    = 1;
	render_pass_info.pSubpasses      = &subpass_description;
	render_pass_info.dependencyCount = 1;
	render_pass_info.pDependencies   = &subpass_dependency;

	if (vkCreateRenderPass(m_device, &render_pass_info, nullptr, &m_render_pass) != VK_SUCCESS)
		LAVA_CORE_ERROR("Failed to create render pass!");

	LAVA_CORE_DEBUG("Created: VkRenderPass");
}

void VulkanWindow::createVulkanDescriptorSetLayout()
{
	VkDescriptorSetLayoutBinding ubo_layout_binding;
	ubo_layout_binding.binding            = 0;
	ubo_layout_binding.descriptorType     = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	ubo_layout_binding.descriptorCount    = 1;
	ubo_layout_binding.stageFlags         = VK_SHADER_STAGE_VERTEX_BIT;
	ubo_layout_binding.pImmutableSamplers = nullptr;

	VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info;
	descriptor_set_layout_create_info.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptor_set_layout_create_info.pNext        = nullptr;
	descriptor_set_layout_create_info.flags        = NULL;
	descriptor_set_layout_create_info.bindingCount = 1;
	descriptor_set_layout_create_info.pBindings    = &ubo_layout_binding;

	if (vkCreateDescriptorSetLayout(m_device, &descriptor_set_layout_create_info, nullptr, &m_descriptor_set_layout) != VK_SUCCESS)
		LAVA_CORE_ERROR("Failed to create descriptor set layout!");

	LAVA_CORE_DEBUG("Created: VkDescriptorSetLayout");
}

void VulkanWindow::createVulkanGraphicsPipeline()
{
	auto vert_shader_code = readShaderFile("C:/dev/repos/Siwyyy/Lava/LavaCore/shaders/basic.vert.spv");
	auto frag_shader_code = readShaderFile("C:/dev/repos/Siwyyy/Lava/LavaCore/shaders/basic.frag.spv");

	m_vert_shader_module = createShaderModule(vert_shader_code);
	m_frag_shader_module = createShaderModule(frag_shader_code);

	VkPipelineShaderStageCreateInfo vert_shader_stage_create_info;
	vert_shader_stage_create_info.sType               = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vert_shader_stage_create_info.pNext               = nullptr;
	vert_shader_stage_create_info.flags               = 0;
	vert_shader_stage_create_info.stage               = VK_SHADER_STAGE_VERTEX_BIT;
	vert_shader_stage_create_info.module              = m_vert_shader_module;
	vert_shader_stage_create_info.pName               = "main";
	vert_shader_stage_create_info.pSpecializationInfo = nullptr;

	VkPipelineShaderStageCreateInfo frag_shader_stage_create_info;
	frag_shader_stage_create_info.sType               = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	frag_shader_stage_create_info.pNext               = nullptr;
	frag_shader_stage_create_info.flags               = 0;
	frag_shader_stage_create_info.stage               = VK_SHADER_STAGE_FRAGMENT_BIT;
	frag_shader_stage_create_info.module              = m_frag_shader_module;
	frag_shader_stage_create_info.pName               = "main";
	frag_shader_stage_create_info.pSpecializationInfo = nullptr;

	VkPipelineShaderStageCreateInfo shader_stage_create_info[] = {vert_shader_stage_create_info,
																																frag_shader_stage_create_info};

	auto binding_description    = Vertex::getBindingDescription();
	auto attribute_descriptions = Vertex::getAttributeDescriptions();

	VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info;
	vertex_input_state_create_info.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertex_input_state_create_info.pNext                           = VK_NULL_HANDLE;
	vertex_input_state_create_info.flags                           = 0;
	vertex_input_state_create_info.vertexBindingDescriptionCount   = 1;
	vertex_input_state_create_info.pVertexBindingDescriptions      = &binding_description;
	vertex_input_state_create_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(attribute_descriptions.size());
	vertex_input_state_create_info.pVertexAttributeDescriptions    = attribute_descriptions.data();

	VkPipelineInputAssemblyStateCreateInfo input_assembly_create_info;
	input_assembly_create_info.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	input_assembly_create_info.pNext                  = nullptr;
	input_assembly_create_info.flags                  = NULL;
	input_assembly_create_info.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	input_assembly_create_info.primitiveRestartEnable = VK_FALSE;

	const std::vector<VkDynamicState> dynamic_states = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};

	VkPipelineDynamicStateCreateInfo dynamic_state_create_info;
	dynamic_state_create_info.sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamic_state_create_info.pNext             = nullptr;
	dynamic_state_create_info.flags             = NULL;
	dynamic_state_create_info.dynamicStateCount = static_cast<uint32_t>(dynamic_states.size());
	dynamic_state_create_info.pDynamicStates    = dynamic_states.data();

	VkPipelineViewportStateCreateInfo viewport_state_create_info;
	viewport_state_create_info.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewport_state_create_info.pNext         = nullptr;
	viewport_state_create_info.flags         = NULL;
	viewport_state_create_info.viewportCount = 1;
	viewport_state_create_info.pViewports    = nullptr; // nullptr because using dynamic state viewport
	viewport_state_create_info.scissorCount  = 1;
	viewport_state_create_info.pScissors     = nullptr; // nullptr because using dynamic state scissor

	VkPipelineRasterizationStateCreateInfo rasterization_state_create_info;
	rasterization_state_create_info.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterization_state_create_info.pNext                   = nullptr;
	rasterization_state_create_info.flags                   = NULL;
	rasterization_state_create_info.depthClampEnable        = VK_FALSE;
	rasterization_state_create_info.rasterizerDiscardEnable = VK_FALSE;
	rasterization_state_create_info.polygonMode             = VK_POLYGON_MODE_FILL;
	rasterization_state_create_info.cullMode                = VK_CULL_MODE_BACK_BIT;
	rasterization_state_create_info.frontFace               = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterization_state_create_info.depthBiasEnable         = VK_FALSE;
	rasterization_state_create_info.depthBiasConstantFactor = 0.0f;
	rasterization_state_create_info.depthBiasClamp          = 0.0f;
	rasterization_state_create_info.depthBiasSlopeFactor    = 0.0f;
	rasterization_state_create_info.lineWidth               = 1.0f;

	VkPipelineMultisampleStateCreateInfo multisample_state_create_info;
	multisample_state_create_info.sType                 = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisample_state_create_info.pNext                 = nullptr;
	multisample_state_create_info.flags                 = NULL;
	multisample_state_create_info.rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT;
	multisample_state_create_info.sampleShadingEnable   = VK_FALSE;
	multisample_state_create_info.minSampleShading      = 1.0f;
	multisample_state_create_info.pSampleMask           = nullptr;
	multisample_state_create_info.alphaToCoverageEnable = VK_FALSE;
	multisample_state_create_info.alphaToOneEnable      = VK_FALSE;

	VkPipelineColorBlendAttachmentState color_blend_attachment_state;
	color_blend_attachment_state.blendEnable         = VK_FALSE;
	color_blend_attachment_state.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
	color_blend_attachment_state.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
	color_blend_attachment_state.colorBlendOp        = VK_BLEND_OP_ADD;
	color_blend_attachment_state.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	color_blend_attachment_state.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	color_blend_attachment_state.alphaBlendOp        = VK_BLEND_OP_ADD;
	color_blend_attachment_state.colorWriteMask      = VK_COLOR_COMPONENT_R_BIT |
																										 VK_COLOR_COMPONENT_G_BIT |
																										 VK_COLOR_COMPONENT_B_BIT |
																										 VK_COLOR_COMPONENT_A_BIT;

	VkPipelineColorBlendStateCreateInfo color_blend_state_create_info;
	color_blend_state_create_info.sType             = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	color_blend_state_create_info.pNext             = nullptr;
	color_blend_state_create_info.flags             = NULL;
	color_blend_state_create_info.logicOpEnable     = VK_FALSE;
	color_blend_state_create_info.logicOp           = VK_LOGIC_OP_COPY;
	color_blend_state_create_info.attachmentCount   = 1;
	color_blend_state_create_info.pAttachments      = &color_blend_attachment_state;
	color_blend_state_create_info.blendConstants[0] = 0.0f;
	color_blend_state_create_info.blendConstants[1] = 0.0f;
	color_blend_state_create_info.blendConstants[2] = 0.0f;
	color_blend_state_create_info.blendConstants[3] = 0.0f;

	VkPipelineLayoutCreateInfo pipeline_layout_create_info;
	pipeline_layout_create_info.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipeline_layout_create_info.pNext                  = nullptr;
	pipeline_layout_create_info.flags                  = NULL;
	pipeline_layout_create_info.setLayoutCount         = 1;
	pipeline_layout_create_info.pSetLayouts            = &m_descriptor_set_layout;
	pipeline_layout_create_info.pushConstantRangeCount = 0;
	pipeline_layout_create_info.pPushConstantRanges    = nullptr;

	if (vkCreatePipelineLayout(m_device, &pipeline_layout_create_info, nullptr, &m_pipeline_layout) != VK_SUCCESS)
		LAVA_CORE_ERROR("Failed to create pipeline layout!");

	LAVA_CORE_DEBUG("Created: VkPipelineLayout");

	VkGraphicsPipelineCreateInfo pipeline_create_info;
	pipeline_create_info.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipeline_create_info.pNext               = nullptr;
	pipeline_create_info.flags               = NULL;
	pipeline_create_info.stageCount          = 2;
	pipeline_create_info.pStages             = shader_stage_create_info;
	pipeline_create_info.pVertexInputState   = &vertex_input_state_create_info;
	pipeline_create_info.pInputAssemblyState = &input_assembly_create_info;
	pipeline_create_info.pTessellationState  = nullptr;
	pipeline_create_info.pViewportState      = &viewport_state_create_info;
	pipeline_create_info.pRasterizationState = &rasterization_state_create_info;
	pipeline_create_info.pMultisampleState   = &multisample_state_create_info;
	pipeline_create_info.pDepthStencilState  = nullptr;
	pipeline_create_info.pColorBlendState    = &color_blend_state_create_info;
	pipeline_create_info.pDynamicState       = &dynamic_state_create_info;
	pipeline_create_info.layout              = m_pipeline_layout;
	pipeline_create_info.renderPass          = m_render_pass;
	pipeline_create_info.subpass             = 0;
	pipeline_create_info.basePipelineHandle  = VK_NULL_HANDLE;
	pipeline_create_info.basePipelineIndex   = -1;

	if (vkCreateGraphicsPipelines(m_device,VK_NULL_HANDLE, 1, &pipeline_create_info, nullptr, &m_pipeline) != VK_SUCCESS)
		LAVA_CORE_ERROR("Failed to create graphics pipeline!");
	LAVA_CORE_DEBUG("Created: VkPipeline");
}

void VulkanWindow::createVulkanCommandPool()
{
	VkCommandPoolCreateInfo command_pool_create_info;
	command_pool_create_info.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	command_pool_create_info.pNext            = nullptr;
	command_pool_create_info.flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	command_pool_create_info.queueFamilyIndex = m_queue_family_indices.graphics.value();

	if (vkCreateCommandPool(m_device, &command_pool_create_info, nullptr, &m_command_pool) != VK_SUCCESS)
		LAVA_CORE_ERROR("Failed to create command pool!");
	LAVA_CORE_DEBUG("Created: VkCommandPool");
}

void VulkanWindow::createVulkanSyncObjects()
{
	m_semaphore_image_available.resize(m_data.max_frames_in_flight);
	m_semaphore_render_finished.resize(m_data.max_frames_in_flight);
	m_fence_in_flight.resize(m_data.max_frames_in_flight);

	VkSemaphoreCreateInfo semaphore_create_info;
	semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	semaphore_create_info.flags = 0;
	semaphore_create_info.pNext = nullptr;

	VkFenceCreateInfo fence_create_info;
	fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
	fence_create_info.pNext = nullptr;

	for (size_t i = 0; i < m_data.max_frames_in_flight; i++)
	{
		if (vkCreateSemaphore(m_device, &semaphore_create_info, nullptr, &m_semaphore_image_available[i]) != VK_SUCCESS ||
				vkCreateSemaphore(m_device, &semaphore_create_info, nullptr, &m_semaphore_render_finished[i]) != VK_SUCCESS ||
				vkCreateFence(m_device, &fence_create_info, nullptr, &m_fence_in_flight[i]) != VK_SUCCESS)
			LAVA_CORE_ERROR("Failed to create semaphores/fence!");
	}
}

// //////////////////////// //

// Multiple usage functions //

// //////////////////////// //
void VulkanWindow::destroyVulkanSwapchain()
{
	for (const VkImageView& image_view : m_image_views)
		vkDestroyImageView(m_device, image_view, nullptr);
	vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);
}

void VulkanWindow::recreateVulkanSwapchain()
{
	int width = 0, height = 0;
	glfwGetFramebufferSize(m_window, &width, &height);
	while (width == 0 || height == 0)
	{
		glfwGetFramebufferSize(m_window, &width, &height);
		glfwWaitEvents();
	}

	vkDeviceWaitIdle(m_device);

	destroyVulkanSwapchain();
	createVulkanSwapchain();
	recreateVulkanFrameBuffers();
}

void VulkanWindow::createVulkanFrameBuffers()
{
	m_framebuffers.resize(m_image_views.size());

	for (size_t i = 0; i < m_image_views.size(); i++)
	{
		const VkImageView attachments[] = {m_image_views[i]};

		VkFramebufferCreateInfo framebuffer_create_info{};
		framebuffer_create_info.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebuffer_create_info.renderPass      = m_render_pass;
		framebuffer_create_info.attachmentCount = 1;
		framebuffer_create_info.pAttachments    = attachments;
		framebuffer_create_info.width           = m_extent_2d.width;
		framebuffer_create_info.height          = m_extent_2d.height;
		framebuffer_create_info.layers          = 1;

		if (vkCreateFramebuffer(m_device, &framebuffer_create_info, nullptr, &m_framebuffers[i]) != VK_SUCCESS)
			LAVA_CORE_ERROR("Failed to create framebuffer!");
	}
}

void VulkanWindow::destroyVulkanFrameBuffers()
{
	for (const auto framebuffer : m_framebuffers)
		vkDestroyFramebuffer(m_device, framebuffer, nullptr);
}

void VulkanWindow::recreateVulkanFrameBuffers()
{
	destroyVulkanFrameBuffers();
	createVulkanFrameBuffers();
}

void VulkanWindow::allocateVulkanCommandBuffers()
{
	m_command_buffers.resize(m_data.max_frames_in_flight);

	VkCommandBufferAllocateInfo command_buffer_allocate_info;
	command_buffer_allocate_info.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	command_buffer_allocate_info.commandPool        = m_command_pool;
	command_buffer_allocate_info.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	command_buffer_allocate_info.commandBufferCount = static_cast<uint32_t>(m_command_buffers.size());
	command_buffer_allocate_info.pNext              = nullptr;

	if (vkAllocateCommandBuffers(m_device, &command_buffer_allocate_info, m_command_buffers.data()) != VK_SUCCESS)
		LAVA_CORE_ERROR("Failed to allocate command buffer!");
}

void VulkanWindow::recordVulkanCommandBuffer(const uint32_t& command_buffer_index_, const uint32_t& image_index_) const
{
	VkCommandBufferBeginInfo command_buffer_begin_info;
	command_buffer_begin_info.sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	command_buffer_begin_info.flags            = 0;
	command_buffer_begin_info.pInheritanceInfo = nullptr;
	command_buffer_begin_info.pNext            = nullptr;

	if (vkBeginCommandBuffer(m_command_buffers[command_buffer_index_], &command_buffer_begin_info) != VK_SUCCESS)
		LAVA_CORE_ERROR("Failed to begin command buffer recording!");

	const VkClearValue clear_value = {{{0.0f,0.0f,0.0f,0.0f}}};
	VkRenderPassBeginInfo render_pass_begin_info;
	render_pass_begin_info.sType             = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	render_pass_begin_info.renderPass        = m_render_pass;
	render_pass_begin_info.framebuffer       = m_framebuffers[image_index_];
	render_pass_begin_info.renderArea.offset = {0,0};
	render_pass_begin_info.renderArea.extent = m_extent_2d;
	render_pass_begin_info.clearValueCount   = 1;
	render_pass_begin_info.pClearValues      = &clear_value;
	render_pass_begin_info.pNext             = nullptr;
	vkCmdBeginRenderPass(m_command_buffers[command_buffer_index_], &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(m_command_buffers[command_buffer_index_], VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);

	VkViewport viewport;
	viewport.x        = 0.0f;
	viewport.y        = 0.0f;
	viewport.width    = static_cast<float>(m_extent_2d.width);
	viewport.height   = static_cast<float>(m_extent_2d.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(m_command_buffers[command_buffer_index_], 0, 1, &viewport);

	VkRect2D scissor;
	scissor.offset = {0,0};
	scissor.extent = m_extent_2d;
	vkCmdSetScissor(m_command_buffers[command_buffer_index_], 0, 1, &scissor);

	const VkBuffer vertex_buffers[] = {m_vertex_buffer};
	const VkDeviceSize offsets[]    = {0};
	vkCmdBindVertexBuffers(m_command_buffers[command_buffer_index_], 0, 1, vertex_buffers, offsets);
	vkCmdBindIndexBuffer(m_command_buffers[command_buffer_index_], m_index_buffer, 0, VK_INDEX_TYPE_UINT16);
	vkCmdBindDescriptorSets(m_command_buffers[command_buffer_index_], VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline_layout, 0, 1, &m_descriptor_sets[m_current_frame], 0, nullptr);

	vkCmdDrawIndexed(m_command_buffers[command_buffer_index_], static_cast<uint32_t>(INDICES.size()), 1, 0, 0, 0);

	vkCmdEndRenderPass(m_command_buffers[command_buffer_index_]);

	if (vkEndCommandBuffer(m_command_buffers[command_buffer_index_]) != VK_SUCCESS)
		LAVA_CORE_ERROR("Failed to record command buffer!");
}

void VulkanWindow::createVulkanBuffer(VkDeviceSize size_, VkBufferUsageFlags usage_, VkMemoryPropertyFlags props_, VkBuffer& buffer_, VkDeviceMemory& buffer_memory_)
{
	VkBufferCreateInfo buffer_create_info;
	buffer_create_info.sType                 = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	buffer_create_info.pNext                 = nullptr;
	buffer_create_info.flags                 = NULL;
	buffer_create_info.size                  = size_;
	buffer_create_info.usage                 = usage_;
	buffer_create_info.sharingMode           = VK_SHARING_MODE_EXCLUSIVE;
	buffer_create_info.queueFamilyIndexCount = 0;
	buffer_create_info.pQueueFamilyIndices   = nullptr;

	if (vkCreateBuffer(m_device, &buffer_create_info, nullptr, &buffer_) != VK_SUCCESS)
		LAVA_CORE_ERROR("failed to create vertex buffer!");

	VkMemoryRequirements memory_requirements;
	vkGetBufferMemoryRequirements(m_device, buffer_, &memory_requirements);

	uint32_t memory_type_index = 0;
	bool found                 = false;
	VkPhysicalDeviceMemoryProperties properties;
	vkGetPhysicalDeviceMemoryProperties(m_gpu, &properties);
	for (uint32_t i = 0; i < properties.memoryTypeCount; i++)
		if ((memory_requirements.memoryTypeBits & (1 << i)) && (properties.memoryTypes[i].propertyFlags & props_) == props_)
		{
			memory_type_index = i;
			found             = true;
		}
	LAVA_ASSERT(found, "Failed to find memory type!")

	VkMemoryAllocateInfo allocate_info;
	allocate_info.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocate_info.pNext           = nullptr;
	allocate_info.allocationSize  = memory_requirements.size;
	allocate_info.memoryTypeIndex = memory_type_index;

	if (vkAllocateMemory(m_device, &allocate_info, nullptr, &buffer_memory_) != VK_SUCCESS)
		LAVA_CORE_ERROR("failed to allocate vertex buffer memory!");

	vkBindBufferMemory(m_device, buffer_, buffer_memory_, 0);
}

void VulkanWindow::copyVulkanBuffer(VkBuffer src_buffer_, VkBuffer dst_buffer_, VkDeviceSize size_)
{
	VkCommandBufferAllocateInfo command_buffer_allocate_info{};
	command_buffer_allocate_info.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	command_buffer_allocate_info.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	command_buffer_allocate_info.commandPool        = m_command_pool;
	command_buffer_allocate_info.commandBufferCount = 1;

	VkCommandBuffer command_buffer;
	vkAllocateCommandBuffers(m_device, &command_buffer_allocate_info, &command_buffer);

	VkCommandBufferBeginInfo command_buffer_begin_info;
	command_buffer_begin_info.sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	command_buffer_begin_info.pNext            = nullptr;
	command_buffer_begin_info.flags            = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	command_buffer_begin_info.pInheritanceInfo = nullptr;

	vkBeginCommandBuffer(command_buffer, &command_buffer_begin_info);

	VkBufferCopy copy_region;
	copy_region.srcOffset = 0;
	copy_region.dstOffset = 0;
	copy_region.size      = size_;
	vkCmdCopyBuffer(command_buffer, src_buffer_, dst_buffer_, 1, &copy_region);

	vkEndCommandBuffer(command_buffer);

	VkSubmitInfo submit_info;
	submit_info.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.pNext                = nullptr;
	submit_info.waitSemaphoreCount   = 0;
	submit_info.pWaitSemaphores      = nullptr;
	submit_info.pWaitDstStageMask    = nullptr;
	submit_info.commandBufferCount   = 1;
	submit_info.pCommandBuffers      = &command_buffer;
	submit_info.signalSemaphoreCount = 0;
	submit_info.pSignalSemaphores    = nullptr;

	vkQueueSubmit(m_graphics_queue, 1, &submit_info,VK_NULL_HANDLE);
	vkQueueWaitIdle(m_graphics_queue);

	vkFreeCommandBuffers(m_device, m_command_pool, 1, &command_buffer);
}

void VulkanWindow::createVulkanVertexBuffer()
{
	VkDeviceSize buffer_size = sizeof(VERTICES[0]) * VERTICES.size();

	VkBuffer staging_buffer;
	VkDeviceMemory staging_buffer_memory;
	createVulkanBuffer(buffer_size,
										 VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
										 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
										 staging_buffer,
										 staging_buffer_memory);

	void* data;
	vkMapMemory(m_device, staging_buffer_memory, 0, buffer_size,NULL, &data);
	memcpy(data, VERTICES.data(), (size_t)buffer_size);
	vkUnmapMemory(m_device, staging_buffer_memory);

	createVulkanBuffer(buffer_size,
										 VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
										 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
										 m_vertex_buffer,
										 m_vertex_buffer_memory);

	copyVulkanBuffer(staging_buffer, m_vertex_buffer, buffer_size);
	vkDestroyBuffer(m_device, staging_buffer, nullptr);
	vkFreeMemory(m_device, staging_buffer_memory, nullptr);
}

void VulkanWindow::createVulkanIndexBuffer()
{
	VkDeviceSize buffer_size = sizeof(INDICES[0]) * INDICES.size();

	VkBuffer staging_buffer;
	VkDeviceMemory staging_buffer_memory;
	createVulkanBuffer(buffer_size,
										 VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
										 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
										 staging_buffer,
										 staging_buffer_memory);

	void* data;
	vkMapMemory(m_device, staging_buffer_memory, 0, buffer_size, 0, &data);
	memcpy(data, INDICES.data(), (size_t)buffer_size);
	vkUnmapMemory(m_device, staging_buffer_memory);

	createVulkanBuffer(buffer_size,
										 VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
										 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
										 m_index_buffer,
										 m_index_buffer_memory);

	copyVulkanBuffer(staging_buffer, m_index_buffer, buffer_size);

	vkDestroyBuffer(m_device, staging_buffer, nullptr);
	vkFreeMemory(m_device, staging_buffer_memory, nullptr);
}

void VulkanWindow::createVulkanUniformBuffers()
{
	VkDeviceSize buffer_size = sizeof(UniformBufferObject);

	m_uniform_buffers.resize(m_data.max_frames_in_flight);
	m_uniform_buffers_memory.resize(m_data.max_frames_in_flight);
	m_uniform_buffers_mapped.resize(m_data.max_frames_in_flight);

	for (size_t i = 0; i < m_data.max_frames_in_flight; i++)
	{
		createVulkanBuffer(buffer_size,
											 VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
											 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
											 m_uniform_buffers[i],
											 m_uniform_buffers_memory[i]);

		vkMapMemory(m_device, m_uniform_buffers_memory[i], 0, buffer_size,NULL, &m_uniform_buffers_mapped[i]);
	}
}

void VulkanWindow::updateUniformBuffer(uint32_t current_frame_)
{
	static auto start_time = std::chrono::high_resolution_clock::now();

	auto current_time = std::chrono::high_resolution_clock::now();
	float time        = std::chrono::duration<float, std::chrono::seconds::period>(current_time - start_time).count();

	UniformBufferObject ubo;
	ubo.model = glm::rotate(glm::mat4(1.0f), m_angle * glm::radians(1.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.view  = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.proj  = glm::perspective(glm::radians(45.0f), (float)m_extent_2d.width / (float)m_extent_2d.height, 0.1f, 10.0f);

	ubo.proj[1][1] *= -1;

	memcpy(m_uniform_buffers_mapped[current_frame_], &ubo, sizeof(ubo));
}

void VulkanWindow::createVulkanDescriptorPool()
{
	VkDescriptorPoolSize pool_size;
	pool_size.type            = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	pool_size.descriptorCount = m_data.max_frames_in_flight;

	VkDescriptorPoolCreateInfo pool_create_info;
	pool_create_info.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	pool_create_info.pNext         = nullptr;
	pool_create_info.flags         = NULL;
	pool_create_info.maxSets       = m_data.max_frames_in_flight;
	pool_create_info.poolSizeCount = 1;
	pool_create_info.pPoolSizes    = &pool_size;

	if (vkCreateDescriptorPool(m_device, &pool_create_info, nullptr, &m_descriptor_pool) != VK_SUCCESS)
		LAVA_CORE_ERROR("Failed to create VkDescriptorPool!");
}

void VulkanWindow::createVulkanDescriptorSets()
{
	std::vector<VkDescriptorSetLayout> layouts(m_data.max_frames_in_flight, m_descriptor_set_layout);
	VkDescriptorSetAllocateInfo set_allocate_info;
	set_allocate_info.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	set_allocate_info.pNext              = nullptr;
	set_allocate_info.descriptorPool     = m_descriptor_pool;
	set_allocate_info.descriptorSetCount = m_data.max_frames_in_flight;
	set_allocate_info.pSetLayouts        = layouts.data();

	m_descriptor_sets.resize(m_data.max_frames_in_flight);
	if (vkAllocateDescriptorSets(m_device, &set_allocate_info, m_descriptor_sets.data()) != VK_SUCCESS)
		LAVA_CORE_ERROR("Failed to allocate descriptor sets!");

	for (size_t i = 0; i < m_data.max_frames_in_flight; i++)
	{
		VkDescriptorBufferInfo buffer_info;
		buffer_info.buffer = m_uniform_buffers[i];
		buffer_info.offset = 0;
		buffer_info.range  = sizeof(UniformBufferObject);

		VkWriteDescriptorSet write_descriptor_set;
		write_descriptor_set.sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write_descriptor_set.pNext            = nullptr;
		write_descriptor_set.dstSet           = m_descriptor_sets[i];
		write_descriptor_set.dstBinding       = 0;
		write_descriptor_set.dstArrayElement  = 0;
		write_descriptor_set.descriptorCount  = 1;
		write_descriptor_set.descriptorType   = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		write_descriptor_set.pImageInfo       = nullptr;
		write_descriptor_set.pBufferInfo      = &buffer_info;
		write_descriptor_set.pTexelBufferView = nullptr;

		vkUpdateDescriptorSets(m_device, 1, &write_descriptor_set, 0, nullptr);
	}
}

// ////////////////////////////////// //
// Initial creation support functions //
// ////////////////////////////////// //

bool VulkanWindow::checkValidationLayerSupport()
{
	uint32_t layer_count;
	vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

	std::vector<VkLayerProperties> available_layers(layer_count);
	vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

	for (const char* layer_name : validation_layers)
	{
		bool layer_found = false;
		for (const auto& layer_properties : available_layers)
			if (strcmp(layer_name, layer_properties.layerName) == 0)
			{
				layer_found = true;
				break;
			}

		if (!layer_found)
			return false;
	}

	return true;
}

VKAPI_ATTR VkBool32 VKAPI_CALL VulkanWindow::debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT msg_severity_,
	VkDebugUtilsMessageTypeFlagsEXT msg_type_,
	const VkDebugUtilsMessengerCallbackDataEXT* p_callback_data_,
	void* p_user_data_)
{
	if (msg_severity_ & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
		LAVA_CORE_DEBUG("<validation layers> {0}", p_callback_data_->pMessage);
	if (msg_severity_ & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
		LAVA_CORE_INFO("<validation layers> {0}", p_callback_data_->pMessage);
	if (msg_severity_ & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
		LAVA_CORE_WARN("<validation layers> {0}", p_callback_data_->pMessage);
	if (msg_severity_ & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
		LAVA_CORE_ERROR("<validation layers> {0}", p_callback_data_->pMessage);
	return false;
}

VkDebugUtilsMessengerCreateInfoEXT VulkanWindow::debugCreateInfo()
{
	VkDebugUtilsMessengerCreateInfoEXT debug_create_info;
	debug_create_info.sType           = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	debug_create_info.pNext           = nullptr;
	debug_create_info.flags           = NULL;
	debug_create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	debug_create_info.messageSeverity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
	//debug_create_info.messageSeverity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
	//debug_create_info.messageSeverity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;
	debug_create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
																	VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
																	VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	debug_create_info.pfnUserCallback = debugCallback;
	debug_create_info.pUserData       = nullptr;
	return debug_create_info;
}

void VulkanWindow::destroyVulkanDebug() const
{
	if (!validation_layers_enabled)
		return;

	const auto destroy_func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
		vkGetInstanceProcAddr(m_instance, "vkDestroyDebugUtilsMessengerEXT"));

	if (destroy_func != nullptr)
		destroy_func(m_instance, m_debug, nullptr);
	else
		LAVA_CORE_ERROR("Failed to destroy Debug Utils Messenger!");
}

std::vector<char> VulkanWindow::readShaderFile(const std::string& filename_)
{
	std::ifstream file(filename_, std::ios::ate | std::ios::binary);

	if (!file.is_open())
		LAVA_CORE_ERROR("Failed to open shader file!");

	const std::streamsize file_size = file.tellg();
	std::vector<char> buffer(file_size);
	file.seekg(0);
	file.read(buffer.data(), file_size);
	file.close();

	return buffer;
}

VkShaderModule VulkanWindow::createShaderModule(const std::vector<char>& code_) const
{
	VkShaderModuleCreateInfo shader_module_create_info;
	shader_module_create_info.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	shader_module_create_info.pNext    = nullptr;
	shader_module_create_info.flags    = NULL;
	shader_module_create_info.codeSize = code_.size();
	shader_module_create_info.pCode    = reinterpret_cast<const uint32_t*>(code_.data());

	VkShaderModule shader_module;
	if (vkCreateShaderModule(m_device, &shader_module_create_info, nullptr, &shader_module) != VK_SUCCESS)
		LAVA_CORE_ERROR("Failed to create shader module!");

	return shader_module;
}
