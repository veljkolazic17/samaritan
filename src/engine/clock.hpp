#pragma once
#include <defines.hpp>

BEGIN_NAMESPACE

namespace Engine
{
	class Clock
	{
	public:
		static Clock& GetInstance() { static Clock instance; return instance; }
		Clock(Clock& other) = delete;
		void operator=(const Clock&) = delete;
	private:
		Clock() {};
		~Clock() {};
	public:
		void Init(void);
		mfloat64 GetTime();
		void Sleep(muint64 miliseconds);
	private:
		mfloat64 m_Frequency = 0;
	};
}

END_NAMESPACE