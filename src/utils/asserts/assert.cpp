#include <utils/asserts/assert.hpp>
#include <utils/logger/log.hpp>

void show_assert(AssertType assertType, const char* file, const char* expression, unsigned int line, const char* message)
{
	if (assertType < AssertType::SIZE)
	{
		const char* assertTypes[2] = { "Soft Assert", "Hard Assert" };
		LogOutput(LogType::LOG_ERROR, LogChannel::General, "%s: %s, message: '%s', in file: %s, line: %d\n", assertTypes[(unsigned int)assertType], expression, message, file, line);
	}
}
