#include "Lava/Lavapch.h"
#include "Lava/Renderer/GraphicsContext.h"

#include "Lava/Renderer/Pipeline.h"

using namespace Lava;

void GraphicsContext::init()
{
	LAVA_CORE_INFO("Initializing Vulkan graphics...");

	createVulkanInstance();
	createVulkanDebug();
	createVulkanDevice();

	createVulkanSwapchain();
	createVulkanRenderPass();
	createVulkanFrameBuffers();

	createVulkanCommandPool();
	allocateVulkanCommandBuffers();
	createVulkanSyncObjects();

	LAVA_CORE_INFO("Vulkan graphics initialization complete");
}

void GraphicsContext::shutdown()
{
	for (size_t i = 0; i < m_frames_in_flight; i++)
	{
		vkDestroySemaphore(m_device, m_semaphore_image_available[i], nullptr);
		vkDestroySemaphore(m_device, m_semaphore_render_finished[i], nullptr);
		vkDestroyFence(m_device, m_fence_in_flight[i], nullptr);
	}
	vkDestroyCommandPool(m_device, m_command_pool, nullptr);

	destroyVulkanFrameBuffers();
	vkDestroyRenderPass(m_device, m_render_pass, nullptr);
	for (const VkImageView& image_view : m_image_views)
		vkDestroyImageView(m_device, image_view, nullptr);
	vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);

	vkDestroyDevice(m_device, nullptr);
	destroyVulkanDebug();
	vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
	vkDestroyInstance(m_instance, nullptr);
}

void GraphicsContext::onUpdate()
{
	glfwPollEvents();
	draw();
}
 
// Drawing //  
  
void GraphicsContext::draw() 
{ 
	for (auto& pipeline : m_pipelines)
		pipeline->updateVulkanUniformBuffer(m_current_frame);   

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
	if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
	{
		LAVA_CORE_ERROR("Failed to acquire swapchain image!");
	}

	vkResetFences(m_device, 1, &m_fence_in_flight[m_current_frame]);
	vkResetCommandBuffer(m_command_buffers[m_current_frame], NULL);
	recordVulkanCommandBuffer(m_current_frame, image_index);

	VkSemaphore wait_semaphores[]      = {m_semaphore_image_available[m_current_frame]};
	VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	VkSemaphore signal_semaphores[]    = {m_semaphore_render_finished[m_current_frame]};

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
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_frame_buffer_resized)
	{
		recreateVulkanSwapchain();
		m_frame_buffer_resized = false;
		return;
	}
	else if (result != VK_SUCCESS) 
	{
		LAVA_CORE_ERROR("Failed to acquire swapchain image!");
	}

	m_current_frame = (m_current_frame + 1) % m_frames_in_flight; 
}

void GraphicsContext::recordVulkanCommandBuffer(const uint32_t& command_buffer_index_, const uint32_t& image_index_) const
{
	VkCommandBufferBeginInfo command_buffer_begin_info;
	command_buffer_begin_info.sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	command_buffer_begin_info.pNext            = nullptr;
	command_buffer_begin_info.flags            = NULL;
	command_buffer_begin_info.pInheritanceInfo = nullptr;

	if (vkBeginCommandBuffer(m_command_buffers[command_buffer_index_], &command_buffer_begin_info) != VK_SUCCESS)
		LAVA_CORE_ERROR("Failed to begin command buffer recording!");

	const VkClearValue clear_value = {{{0.01f,0.01f,0.01f,0.0f}}};
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

	for (auto& pipeline : m_pipelines)
		pipeline->draw(m_command_buffers[command_buffer_index_], m_current_frame);

	vkCmdEndRenderPass(m_command_buffers[command_buffer_index_]);

	if (vkEndCommandBuffer(m_command_buffers[command_buffer_index_]) != VK_SUCCESS)
		LAVA_CORE_ERROR("Failed to record command buffer!");
}

// /////////////////////////////////////////// //

// Instance //

void GraphicsContext::createVulkanInstance()
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

	if (s_validation_layers_enabled)
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
	if (s_validation_layers_enabled && !checkValidationLayerSupport())
		LAVA_CORE_ERROR("Validation layers requested, but not available!");
	else if (!s_validation_layers_enabled)
		LAVA_CORE_INFO("Validation layers disabled");

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
	if (s_validation_layers_enabled)
	{
		instance_create_info.enabledLayerCount   = static_cast<uint32_t>(s_validation_layers.size());
		instance_create_info.ppEnabledLayerNames = s_validation_layers.data();
		debug_create_info                        = debugCreateInfo();
		instance_create_info.pNext               = &debug_create_info;
	}

	if (vkCreateInstance(&instance_create_info, nullptr, &m_instance) != VK_SUCCESS)
		LAVA_CORE_ERROR("Failed to create instance!");

	LAVA_CORE_DEBUG("Created: VkInstance");
}

void GraphicsContext::createVulkanDebug()
{
	if (!s_validation_layers_enabled)
		return;

	const auto create_func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
		vkGetInstanceProcAddr(m_instance, "vkCreateDebugUtilsMessengerEXT"));

	LAVA_ASSERT(create_func, "Failed to get function vkCreateDebugUtilsMessengerEXT!")

	VkDebugUtilsMessengerCreateInfoEXT debug_info = debugCreateInfo();

	if (create_func(m_instance, &debug_info, nullptr, &m_debug) != VK_SUCCESS)
		LAVA_CORE_ERROR("Failed to create debug utils messenger");

	LAVA_CORE_DEBUG("Created: VkDebugUtilsMessenger");
}

void GraphicsContext::createVulkanDevice()
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

	if (s_validation_layers_enabled)
	{
		device_create_info.enabledLayerCount   = static_cast<uint32_t>(s_validation_layers.size());
		device_create_info.ppEnabledLayerNames = s_validation_layers.data();
	}

	if (vkCreateDevice(m_gpu, &device_create_info, nullptr, &m_device) != VK_SUCCESS)
		LAVA_CORE_ERROR("Failed to create logical device!");

	LAVA_CORE_DEBUG("Created: VkDevice");

	// Get handles for graphics and presentation queues
	vkGetDeviceQueue(m_device, m_queue_family_indices.graphics.value(), 0, &m_graphics_queue);
	vkGetDeviceQueue(m_device, m_queue_family_indices.present.value(), 0, &m_present_queue);
}

// Swapchain //

void GraphicsContext::createVulkanSwapchain()
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

void GraphicsContext::destroyVulkanSwapchain()
{
	for (const VkImageView& image_view : m_image_views)
		vkDestroyImageView(m_device, image_view, nullptr);
	vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);
}

void GraphicsContext::recreateVulkanSwapchain()
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

void GraphicsContext::createVulkanRenderPass()
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

void GraphicsContext::createVulkanFrameBuffers()
{
	m_framebuffers.resize(m_image_views.size());

	for (size_t i = 0; i < m_image_views.size(); i++)
	{
		const VkImageView attachments[] = {m_image_views[i]};

		VkFramebufferCreateInfo framebuffer_create_info;
		framebuffer_create_info.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebuffer_create_info.pNext           = nullptr;
		framebuffer_create_info.flags           = NULL;
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

void GraphicsContext::destroyVulkanFrameBuffers()
{
	for (const auto framebuffer : m_framebuffers)
		vkDestroyFramebuffer(m_device, framebuffer, nullptr);
}

void GraphicsContext::recreateVulkanFrameBuffers()
{
	destroyVulkanFrameBuffers();
	createVulkanFrameBuffers();
}

// Command buffers //

void GraphicsContext::createVulkanCommandPool()
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

void GraphicsContext::allocateVulkanCommandBuffers()
{
	m_command_buffers.resize(m_frames_in_flight);

	VkCommandBufferAllocateInfo command_buffer_allocate_info;
	command_buffer_allocate_info.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	command_buffer_allocate_info.commandPool        = m_command_pool;
	command_buffer_allocate_info.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	command_buffer_allocate_info.commandBufferCount = static_cast<uint32_t>(m_command_buffers.size());
	command_buffer_allocate_info.pNext              = nullptr;

	if (vkAllocateCommandBuffers(m_device, &command_buffer_allocate_info, m_command_buffers.data()) != VK_SUCCESS)
		LAVA_CORE_ERROR("Failed to allocate command buffer!");
}

void GraphicsContext::createVulkanSyncObjects()
{
	m_semaphore_image_available.resize(m_frames_in_flight);
	m_semaphore_render_finished.resize(m_frames_in_flight);
	m_fence_in_flight.resize(m_frames_in_flight);

	VkSemaphoreCreateInfo semaphore_create_info;
	semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	semaphore_create_info.flags = 0;
	semaphore_create_info.pNext = nullptr;

	VkFenceCreateInfo fence_create_info;
	fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
	fence_create_info.pNext = nullptr;

	for (size_t i = 0; i < m_frames_in_flight; i++)
	{
		if (vkCreateSemaphore(m_device, &semaphore_create_info, nullptr, &m_semaphore_image_available[i]) != VK_SUCCESS ||
				vkCreateSemaphore(m_device, &semaphore_create_info, nullptr, &m_semaphore_render_finished[i]) != VK_SUCCESS ||
				vkCreateFence(m_device, &fence_create_info, nullptr, &m_fence_in_flight[i]) != VK_SUCCESS)
			LAVA_CORE_ERROR("Failed to create semaphores/fence!");
	}
}

// /////////////////////////////////////////// //

// Debug helper functions //

bool GraphicsContext::checkValidationLayerSupport()
{
	uint32_t layer_count;
	vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

	std::vector<VkLayerProperties> available_layers(layer_count);
	vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

	for (const char* layer_name : s_validation_layers)
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

VKAPI_ATTR VkBool32 VKAPI_CALL GraphicsContext::debugCallback(
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

VkDebugUtilsMessengerCreateInfoEXT GraphicsContext::debugCreateInfo()
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

void GraphicsContext::destroyVulkanDebug() const
{
	if (!s_validation_layers_enabled)
		return;

	const auto destroy_func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
		vkGetInstanceProcAddr(m_instance, "vkDestroyDebugUtilsMessengerEXT"));

	if (destroy_func != nullptr)
		destroy_func(m_instance, m_debug, nullptr);
	else
		LAVA_CORE_ERROR("Failed to destroy Debug Utils Messenger!");
}
