#pragma once
#include "Lava/Application.h"
#include "Lava/Resources.h"

namespace Lava
{
	enum class ShaderStage
	{
		VertexShader,
		FragmentShader
	};

	class Shader
	{
	public:
		Shader() = delete;

		Shader(ShaderStage stage_, const std::string& filename_)
			: m_stage(stage_)
		{
			m_code          = readShaderFile(filename_);
			m_shader_module = createShaderModule(m_code);
		}

		~Shader();

		inline auto getShaderModule() const { return m_shader_module; }

	private:
		ShaderStage m_stage;
		std::vector<char> m_code;
		VkShaderModule m_shader_module;

		std::vector<char> readShaderFile(const std::string& filename_) const
		{
			std::filesystem::path shaders_path = Resources::getDir(ResourceDir::Shaders);
			std::ifstream file(shaders_path /= filename_, std::ios::ate | std::ios::binary);

			if (!file.is_open())
				LAVA_CORE_ERROR("Failed to open shader file!");

			const std::streamsize file_size = file.tellg();
			std::vector<char> buffer(file_size);
			file.seekg(0);
			file.read(buffer.data(), file_size);
			file.close();

			return buffer;
		}

		VkShaderModule createShaderModule(const std::vector<char>& code_) const
		{
			VkShaderModuleCreateInfo shader_module_create_info;
			shader_module_create_info.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			shader_module_create_info.pNext    = nullptr;
			shader_module_create_info.flags    = NULL;
			shader_module_create_info.codeSize = code_.size();
			shader_module_create_info.pCode    = reinterpret_cast<const uint32_t*>(code_.data());

			VkDevice device = Application::getInstance().getWindow().getContext()->getDevice();
			VkShaderModule shader_module;
			if (vkCreateShaderModule(device, &shader_module_create_info, nullptr, &shader_module) != VK_SUCCESS)
				LAVA_CORE_ERROR("Failed to create shader module!");

			return shader_module;
		}
	};
}
