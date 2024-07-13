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
		mfloat64 GetTime();
		void Sleep(muint64 miliseconds);
	private:
		mfloat64 m_Frequency = 0;
	};
}

END_NAMESPACE