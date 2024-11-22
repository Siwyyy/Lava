#pragma once

#include <chrono>

namespace Lava
{
	class Time
	{
	public:
		Time()
		{
			m_previous_frame = m_timer.now();
			m_delta_time     = 0.f;
		}

		~Time() = default;

		static auto deltaTime() { return s_instance->m_delta_time; }

	private:
		inline static Time* s_instance = nullptr;

		std::chrono::high_resolution_clock m_timer;
		std::chrono::steady_clock::time_point m_previous_frame;
		float m_delta_time;

		static void onUpdate()
		{
			auto now                     = s_instance->m_timer.now();
			s_instance->m_delta_time     = static_cast<float>(std::chrono::duration_cast<std::chrono::microseconds>(now - s_instance->m_previous_frame).count()) / 1000000.f;
			s_instance->m_previous_frame = now;
		}

		friend class Application;
	};
}
