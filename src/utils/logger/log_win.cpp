#ifdef TARGET_WIN
#include <utils/logger/log.hpp>
#include <Windows.h>

void TTYOutput(const char* message)
{
	OutputDebugStringA(message);
}

#endif