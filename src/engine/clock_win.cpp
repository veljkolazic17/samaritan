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
		m_Frequency = 1.0 / (mfloat64)frequency.QuadPart;
	}

	mfloat64 Clock::GetTime()
	{
		LARGE_INTEGER now_time;
		QueryPerformanceCounter(&now_time);
		return (mfloat64)now_time.QuadPart * m_Frequency;
	}

	void Clock::Sleep(muint64 miliseconds)
	{
		::Sleep(miliseconds);
	}
}

END_NAMESPACE
#endif