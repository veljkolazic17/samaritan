#include <utils/asserts/assert.hpp>
#include <utils/logger/log.hpp>
#include <cstdarg>
#include <cstdio>

void show_assert(AssertType assertType, const char* file, const char* expression, unsigned int line, const char* message, ...)
{
	if (assertType < AssertType::SIZE)
	{
		char formatted[1024];
		va_list args;
		va_start(args, message);
		vsnprintf(formatted, sizeof(formatted), message, args);
		va_end(args);

		const char* assertTypes[2] = { "Soft Assert", "Hard Assert" };
		LogOutput(LogType::LOG_ERROR, LogChannel::General, "%s: %s, message: '%s', in file: %s, line: %d\n", assertTypes[(unsigned int)assertType], expression, formatted, file, line);
	}
}
