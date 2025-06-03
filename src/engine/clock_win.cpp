#ifdef TARGET_WIN
#include <engine/clock.hpp>
#include <windows.h>

BEGIN_NAMESPACE

namespace Engine
{
	void Clock::Init(void)
	{
		LARGE_INTEGER frequency;
		QueryPerformanceFrequency(&frequency);
		m_Frequency = 1.0 / (smfloat64)frequency.QuadPart;
	}

	Time Clock::GetTime()
	{
		LARGE_INTEGER now_time;
		QueryPerformanceCounter(&now_time);
		return (smfloat64)now_time.QuadPart * m_Frequency;
	}

	void Clock::Sleep(Time miliseconds)
	{
		::Sleep(miliseconds);
	}
}

END_NAMESPACE
#endif