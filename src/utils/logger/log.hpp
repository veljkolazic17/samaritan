#pragma once

#include <defines.hpp>
#include <utils/logger/logchannels.hpp>

#define LOG_INFO_ENABLED        true
#define LOG_DEBUG_ENABLED       true
#define LOG_WARINING_ENABLED    true
#define LOG_ERROR_ENABLED       true
#define LOG_CRASH_ENABLED       true

enum class LogType
{
    LOG_INFO,
    LOG_DEBUG,
    LOG_WARNING,
    LOG_ERROR,
    SIZE = 4
};

void TTYOutput(const char* message);
void LogOutput(LogType logType, LogChannel channel, const char* message, ...);
void LogOutput(LogType logType, const char* message, ...);

#ifdef _MSC_VER
#include <cstdarg>
#define variable_arguments_list va_list
#else
#define variable_arguments_list __builtin_va_list
#endif


#if LOG_INFO_ENABLED == true
    #define LogInfo(channel, message, ...) LogOutput(LogType::LOG_INFO, channel, message, ##__VA_ARGS__);
    #define LogInfoTTY(message, ...) LogOutput(LogType::LOG_INFO, message, ##__VA_ARGS__);
#else
    #define LogInfoRemote(channel, message, ...)
    #define LogInfo(message, ...)
#endif

#if LOG_DEBUG_ENABLED == true
    #define LogDebug(channel, message, ...) LogOutput(LogType::LOG_DEBUG, channel, message, ##__VA_ARGS__);
    #define LogDebugTTY(message, ...) LogOutput(LogType::LOG_DEBUG, message, ##__VA_ARGS__);
#else
    #define LogDebugRemote(channel, message, ...)
    #define LogDebug(message, ...)
#endif

#if LOG_WARINING_ENABLED == true
    #define LogWarning(channel, message, ...) LogOutput(LogType::LOG_WARNING, channel, message, ##__VA_ARGS__);
    #define LogWarningTTY(message, ...) LogOutput(LogType::LOG_WARNING, message, ##__VA_ARGS__);
#else
    #define LogWarningRemote(channel, message, ...)
    #define LogWarning(message, ...)
#endif

#if LOG_ERROR_ENABLED == true
    #define LogError(channel, message, ...) LogOutput(LogType::LOG_ERROR, channel, message, ##__VA_ARGS__);
    #define LogErrorTTY(message, ...) LogOutput(LogType::LOG_ERROR, message, ##__VA_ARGS__);
#else
    #define LogErrorRemote(channel, message, ...)
    #define LogError(message, ...)
#endif
