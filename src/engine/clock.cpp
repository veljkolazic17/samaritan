#include <engine/clock.hpp>

BEGIN_NAMESPACE

namespace Engine
{
	void Clock::Update()
	{
		if (m_StartTime != 0)
		{
			m_Elapsed = GetTime() - m_StartTime;
		}
	}

	void Clock::Start()
	{
		m_StartTime = GetTime();
	}

	void Clock::Stop()
	{
		m_StartTime = 0;
	}
}

END_NAMESPACE