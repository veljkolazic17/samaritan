#pragma once
#include <defines.hpp>

#define EVENT_CONTEXT_SIZE	128
#define MAX_EVENT_CODES		16384
#define InvalidCallback		nullptr
#define InvalidCaller		nullptr

BEGIN_NAMESPACE

namespace Events
{
	enum class EventCode
	{
		SIZE
	};

	struct EventContext
	{
		// Max size 128b
		union 
		{
			mint64 i64[2];
			muint64 u64[2];
			mfloat64 f64[2];

			mint32 i32[4];
			muint32 u32[4];
			mfloat32 f32[4];

			mint16 i16[8];
			muint16 u16[8];

			mint8 i8[16];
			muint8 u8[16];
		} 
		m_Data;
	};

	template<typename T>
	using EventCallback = mbool(*)(T& caller, T& listener, const EventContext& context);

	template<typename T>
	struct Event
	{
		T* m_Listener = nullptr;
		EventContext m_Context;
		EventCallback<T> m_Callback = InvalidCallback;
	};
}

END_NAMESPACE