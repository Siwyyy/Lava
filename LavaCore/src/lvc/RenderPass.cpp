#include "lvc/RenderPass.hpp"

#include "lvc/Device.hpp"
#include "lvc/Swapchain.hpp"

#include <vulkan/vulkan.h>

#include <iostream>
#include <stdexcept>

using namespace lvc;

RenderPass::RenderPass(const Device* device, const Swapchain* swapchain)
	: m_device(device->hDevice())
	, m_swapchain_image_format(swapchain->hFormat())
	, m_render_pass(VK_NULL_HANDLE)
{
	VkAttachmentDescription attachment_description;
	attachment_description.flags          = 0;
	attachment_description.format         = m_swapchain_image_format;
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
	subpass_description.colorAttachmentCount    = 1;
	subpass_description.pColorAttachments       = &attachment_reference;
	subpass_description.pResolveAttachments     = VK_NULL_HANDLE;
	subpass_description.pDepthStencilAttachment = VK_NULL_HANDLE;
	subpass_description.preserveAttachmentCount = 0;

	VkSubpassDependency subpass_dependency;
	subpass_dependency.srcSubpass    = VK_SUBPASS_EXTERNAL;
	subpass_dependency.dstSubpass    = 0;
	subpass_dependency.srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subpass_dependency.dstStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subpass_dependency.srcAccessMask = 0;
	subpass_dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
																		 VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	subpass_dependency.dependencyFlags = 0;

	VkRenderPassCreateInfo render_pass_info{};
	render_pass_info.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	render_pass_info.attachmentCount = 1;
	render_pass_info.pAttachments    = &attachment_description;
	render_pass_info.subpassCount    = 1;
	render_pass_info.pSubpasses      = &subpass_description;
	render_pass_info.dependencyCount = 1;
	render_pass_info.pDependencies   = &subpass_dependency;

	if (vkCreateRenderPass(m_device, &render_pass_info, nullptr, &m_render_pass) != VK_SUCCESS)
		throw std::runtime_error("err: Failed to create render pass!\n");
}

RenderPass::~RenderPass()
{
	vkDestroyRenderPass(m_device, m_render_pass, nullptr);
	std::clog << "Successfully destroyed render pass\n";
}