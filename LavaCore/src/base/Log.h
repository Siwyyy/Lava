#pragma once

#include "Core.h"

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

namespace Lava
{
	class LAVA_API Log
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
#define LAVA_CORE_TRACE(...)     ::Lava::Log::getCoreLogger()->trace(__VA_ARGS__)
#define LAVA_CORE_DEBUG(...)     ::Lava::Log::getCoreLogger()->debug(__VA_ARGS__)
#define LAVA_CORE_INFO(...)      ::Lava::Log::getCoreLogger()->info(__VA_ARGS__)
#define LAVA_CORE_WARN(...)      ::Lava::Log::getCoreLogger()->warn(__VA_ARGS__)
#define LAVA_CORE_ERROR(...)     ::Lava::Log::getCoreLogger()->error(__VA_ARGS__)
#define LAVA_CORE_CRITICAL(...)  ::Lava::Log::getCoreLogger()->critical(__VA_ARGS__)
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
#define LAVA_CLIENT_TRACE(...)     ::Lava::Log::getCoreLogger()->trace(__VA_ARGS__)
#define LAVA_CLIENT_DEBUG(...)     ::Lava::Log::getCoreLogger()->debug(__VA_ARGS__)
#define LAVA_CLIENT_INFO(...)      ::Lava::Log::getCoreLogger()->info(__VA_ARGS__)
#define LAVA_CLIENT_WARN(...)      ::Lava::Log::getCoreLogger()->warn(__VA_ARGS__)
#define LAVA_CLIENT_ERROR(...)     ::Lava::Log::getCoreLogger()->error(__VA_ARGS__)
#define LAVA_CLIENT_CRITICAL(...)  ::Lava::Log::getCoreLogger()->critical(__VA_ARGS__)
#endif
