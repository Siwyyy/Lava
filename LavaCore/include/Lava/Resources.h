#pragma once

namespace Lava
{
	enum class ResourceDir
	{
		Shaders,
		Models
	};

	struct Resources
	{
	public:
		static void setDir(ResourceDir dir_, const std::filesystem::path& path_)
		{
			switch (dir_)
			{
			case ResourceDir::Shaders:
				m_shaders_dir = path_;
				break;
			case ResourceDir::Models:
				m_models_dir = path_;
				break;
			}
		}

		static std::filesystem::path getDir(ResourceDir dir_)
		{
			switch (dir_)
			{
			case ResourceDir::Shaders:
				return m_shaders_dir;
			case ResourceDir::Models:
				return m_models_dir;
			}
			return "Error/Wrong/Dir";
		}

	private:
		static std::filesystem::path m_shaders_dir;
		static std::filesystem::path m_models_dir;
	};
}
