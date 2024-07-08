#pragma once

#include<vulkan/vulkan.h>

#include <string>
#include <vector>

namespace lava
{
	class RenderPass;

	class GraphicsPipeline
	{
	public:
		GraphicsPipeline(const VkDevice& t_device,
										 const VkExtent2D& t_swapchain_extent,
										 const VkRenderPass& t_render_pass,
										 RenderPass& t_rpass);
		GraphicsPipeline() = delete;
		~GraphicsPipeline();

		inline const VkPipeline& hPipeline() const { return m_pipeline; }

	private:
		const VkDevice& m_device;
		const VkExtent2D& m_swapchain_extent;
		const VkRenderPass& m_render_pass;

		VkShaderModule m_vert_shader_module;
		VkShaderModule m_frag_shader_module;

		VkPipelineLayout m_pipeline_layout;
		VkPipeline m_pipeline;

		VkShaderModule createShaderModule(const std::vector<char>& code) const;
		static std::vector<char> readShaderFile(const std::string& filename);
	};
}
