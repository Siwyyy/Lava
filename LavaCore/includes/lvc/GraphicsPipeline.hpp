#pragma once

#include<vulkan/vulkan.h>

#include <string>
#include <vector>

namespace lvc
{
	class Device;
	class Swapchain;
	class RenderPass;

	class GraphicsPipeline
	{
	public:
		explicit GraphicsPipeline(Device* device, Swapchain* swapchain, RenderPass* render_pass);
		GraphicsPipeline() = delete;
		~GraphicsPipeline();

	private:
		VkDevice& m_device;
		VkExtent2D& m_swapchain_extent;
		VkRenderPass& m_render_pass;

		VkShaderModule m_vert_shader_module;
		VkShaderModule m_frag_shader_module;

		VkPipelineLayout m_pipeline_layout;
		VkPipeline m_pipeline;

		VkShaderModule createShaderModule(const std::vector<char>& code) const;
		static std::vector<char> readShaderFile(const std::string& filename);
	};
}
