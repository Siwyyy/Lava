#pragma once

#include "Core.hpp"

#include "spdlog/spdlog.h"

#include <memory>

namespace lava
{
	class LAVA_BUILD_DLL Log
	{
	public:
		Log() = delete;

		static void init();

		inline static std::shared_ptr<spdlog::logger>& getCoreLogger() { return s_core_logger; }
		inline static std::shared_ptr<spdlog::logger>& getClientLogger() { return s_client_logger; }

	private:
		static std::shared_ptr<spdlog::logger> s_core_logger;
		static std::shared_ptr<spdlog::logger> s_client_logger;
	};
}

// Core logger macros
#ifdef NDEBUG
#define LAVA_CORE_TRACE(...)
#define LAVA_CORE_DEBUG(...)
#define LAVA_CORE_INFO(...)
#define LAVA_CORE_WARN(...)
#define LAVA_CORE_ERROR(...)
#define LAVA_CORE_CRITICAL(...)
#else
#define LAVA_CORE_TRACE(...)     ::lava::Log::getCoreLogger()->trace(__VA_ARGS__)
#define LAVA_CORE_DEBUG(...)     ::lava::Log::getCoreLogger()->debug(__VA_ARGS__)
#define LAVA_CORE_INFO(...)      ::lava::Log::getCoreLogger()->info(__VA_ARGS__)
#define LAVA_CORE_WARN(...)      ::lava::Log::getCoreLogger()->warn(__VA_ARGS__)
#define LAVA_CORE_ERROR(...)     ::lava::Log::getCoreLogger()->error(__VA_ARGS__)
#define LAVA_CORE_CRITICAL(...)  ::lava::Log::getCoreLogger()->critical(__VA_ARGS__)
#endif

// Client logger macros
#ifdef NDEBUG
#define LAVA_CLIENT_TRACE(...)
#define LAVA_CLIENT_DEBUG(...)
#define LAVA_CLIENT_INFO(...)
#define LAVA_CLIENT_WARN(...)
#define LAVA_CLIENT_ERROR(...)
#define LAVA_CLIENT_CRITICAL(...)
#else
#define LAVA_CLIENT_TRACE(...)     ::lava::Log::getCoreLogger()->trace(__VA_ARGS__)
#define LAVA_CLIENT_DEBUG(...)     ::lava::Log::getCoreLogger()->debug(__VA_ARGS__)
#define LAVA_CLIENT_INFO(...)      ::lava::Log::getCoreLogger()->info(__VA_ARGS__)
#define LAVA_CLIENT_WARN(...)      ::lava::Log::getCoreLogger()->warn(__VA_ARGS__)
#define LAVA_CLIENT_ERROR(...)     ::lava::Log::getCoreLogger()->error(__VA_ARGS__)
#define LAVA_CLIENT_CRITICAL(...)  ::lava::Log::getCoreLogger()->critical(__VA_ARGS__)
#endif
