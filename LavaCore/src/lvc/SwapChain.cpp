#include "lvc/Swapchain.hpp"

#include "lvc/Device.hpp"
#include "lvc/PhysicalDevice.hpp"
#include "lvc/Window.hpp"

#include <algorithm>
#include <iostream>
#include <stdexcept>

using namespace lvc;

Swapchain::Swapchain(Device* device, Window* window)
	: m_device(device->hDevice())
	, m_physical_device(device->physicalDevice()->physical())
	, m_window(window->hWindow())
	, m_surface(window->hSurface())
	, m_graphics_family(device->physicalDevice()->indices()->graphics_family.value())
	, m_present_family(device->physicalDevice()->indices()->present_family.value())
{
	querySwapchainSupport();
	setExtent2D();
	setSurfaceFormat();
	setSurfacePresentMode();
	createSwapchain();
}

Swapchain::~Swapchain()
{
	for (const VkImageView& image_view : m_image_views)
		vkDestroyImageView(m_device, image_view, nullptr);
	vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);
	std::clog << "Successfully destroyed swapchain\n";
}

void Swapchain::querySwapchainSupport()
{
	// Extent
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_physical_device, m_surface, &m_surface_capabilities);

	// Surface format
	uint32_t format_count;
	vkGetPhysicalDeviceSurfaceFormatsKHR(m_physical_device, m_surface, &format_count, nullptr);
	m_surface_formats.resize(format_count);
	vkGetPhysicalDeviceSurfaceFormatsKHR(m_physical_device, m_surface, &format_count, m_surface_formats.data());

	// Surface present mode
	uint32_t mode_count;
	vkGetPhysicalDeviceSurfacePresentModesKHR(m_physical_device, m_surface, &mode_count, nullptr);
	m_present_modes.resize(mode_count);
	vkGetPhysicalDeviceSurfacePresentModesKHR(m_physical_device, m_surface, &mode_count, m_present_modes.data());

	if (m_surface_formats.empty() || m_present_modes.empty())
		throw::std::runtime_error("err: Device details not supported! SWAPCHAIN cannot be created!");
}

void Swapchain::setExtent2D()
{
	if (m_surface_capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
	{
		m_extent_2d = m_surface_capabilities.currentExtent;
		return;
	}

	int width_int, height_int;
	glfwGetFramebufferSize(m_window, &width_int, &height_int);

	const uint32_t width  = std::clamp(static_cast<uint32_t>(width_int), m_surface_capabilities.minImageExtent.width, m_surface_capabilities.maxImageExtent.width);
	const uint32_t height = std::clamp(static_cast<uint32_t>(height_int), m_surface_capabilities.minImageExtent.height, m_surface_capabilities.maxImageExtent.height);

	m_extent_2d = {width,height};
}

void Swapchain::setSurfaceFormat()
{
	for (const auto& format : m_surface_formats)
		if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			m_surface_format = format;
			return;
		}

	m_surface_format = m_surface_formats[0];
}

void Swapchain::setSurfacePresentMode()
{
	for (const auto& mode : m_present_modes)
		if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			m_present_mode = mode;
			return;
		}
	m_present_mode = VK_PRESENT_MODE_FIFO_KHR;
}

void Swapchain::createSwapchain()
{
	uint32_t image_count = m_surface_capabilities.minImageCount + 1;
	if (m_surface_capabilities.maxImageCount > 0 && image_count < m_surface_capabilities.maxImageCount)
		image_count = m_surface_capabilities.maxImageCount;

	VkSwapchainCreateInfoKHR create_info{};
	create_info.sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	create_info.surface          = m_surface;
	create_info.minImageCount    = image_count;
	create_info.imageFormat      = m_surface_format.format;
	create_info.imageColorSpace  = m_surface_format.colorSpace;
	create_info.imageExtent      = m_extent_2d;
	create_info.imageArrayLayers = 1;
	create_info.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	const uint32_t queue_family_indices[] = {m_graphics_family,m_present_family};
	if (m_graphics_family != m_present_family)
	{
		create_info.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
		create_info.queueFamilyIndexCount = 2;
		create_info.pQueueFamilyIndices   = queue_family_indices;
	}
	else
	{
		create_info.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
		create_info.queueFamilyIndexCount = 0;
		create_info.pQueueFamilyIndices   = nullptr;
	}

	create_info.preTransform   = m_surface_capabilities.currentTransform;
	create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	create_info.presentMode    = m_present_mode;
	create_info.clipped        = VK_TRUE;
	create_info.oldSwapchain   = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(m_device, &create_info, nullptr, &m_swapchain) != VK_SUCCESS)
		throw std::runtime_error("err: Failed to create swapchain!\n");

	vkGetSwapchainImagesKHR(m_device, m_swapchain, &image_count, nullptr);
	m_images.resize(image_count);
	vkGetSwapchainImagesKHR(m_device, m_swapchain, &image_count, m_images.data());
}

void Swapchain::createImageViews()
{
	m_image_views.resize(m_images.size());
	for (size_t i = 0; i < m_image_views.size(); i++)
	{
		VkImageViewCreateInfo create_info{};
		create_info.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		create_info.image                           = m_images[i];
		create_info.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
		create_info.format                          = m_surface_format.format;
		create_info.components.r                    = VK_COMPONENT_SWIZZLE_IDENTITY;
		create_info.components.g                    = VK_COMPONENT_SWIZZLE_IDENTITY;
		create_info.components.b                    = VK_COMPONENT_SWIZZLE_IDENTITY;
		create_info.components.a                    = VK_COMPONENT_SWIZZLE_IDENTITY;
		create_info.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
		create_info.subresourceRange.baseMipLevel   = 0;
		create_info.subresourceRange.levelCount     = 1;
		create_info.subresourceRange.baseArrayLayer = 0;
		create_info.subresourceRange.layerCount     = 1;

		if (vkCreateImageView(m_device, &create_info, nullptr, &m_image_views[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create image views!");
		}
	}
}
