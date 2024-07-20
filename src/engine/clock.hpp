#pragma once
#include <defines.hpp>
#include <objecttemplates/singleton.hpp>

BEGIN_NAMESPACE

namespace Engine
{
	class Clock SINGLETON(Clock)
	{
	private:
		SINGLETON_CONSTRUCTOR(Clock);
	public:
		void Init(void);
		inline Time GetStartTime() { return m_StartTime; }
		inline Time GetElapsed() { return m_Elapsed; }

		// Platform specific
		Time GetTime();
		void Sleep(Time miliseconds);

		// Not platform specific
		void Update();
		void Start();
		void Stop();

	private:
		mfloat64 m_Frequency = 0;
		Time m_StartTime = 0;
		Time m_Elapsed = 0;
	};
}

END_NAMESPACE