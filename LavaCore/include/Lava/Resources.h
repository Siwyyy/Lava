#pragma once

namespace Lava
{
	enum class ResourceDir
	{
		Shaders
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
			}
		}

		static std::filesystem::path getDir(ResourceDir dir_)
		{
			switch (dir_)
			{
			case ResourceDir::Shaders:
				return m_shaders_dir;
			}
			return "Error/Wrong/Dir";
		}

	private:
		static std::filesystem::path m_shaders_dir;
	};
}
