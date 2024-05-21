#include <defines.hpp>
#include <utils/logger/log.hpp>
#include <utils/logger/remotelogger/redislogger.hpp>

#include <cstring>
#include <iostream>
#include <format>

void LogOutput(LogType logType, const char* message, ...)
{
    const char* logMessageHeader = nullptr;
    switch (logType)
    {
    case LogType::LOG_INFO:
    {
        logMessageHeader = "[INFO]: ";
        break;
    }
    case LogType::LOG_DEBUG:
    {
        logMessageHeader = "[DEBUG]: ";
        break;
    }
    case LogType::LOG_WARNING:
    {
        logMessageHeader = "[WARNING]: ";
        break;
    }
    case LogType::LOG_ERROR:
    {
        logMessageHeader = "[ERROR]: ";
        break;
    }
    default:
    {
        logMessageHeader = "[UNKNOWN]: ";
        break;
    }
    }
    char outputMessage[2048];
    std::memset(outputMessage, 0, sizeof(outputMessage));

    variable_arguments_list arguments;
    va_start(arguments, message);
    vsnprintf(outputMessage, 32000, message, arguments);
    va_end(arguments);

    std::string ttyMessage = std::format("{}{}\n", logMessageHeader, outputMessage);
    TTYOutput(ttyMessage.data());
}

void LogOutput(LogType logType, LogChannel channel, const char* message, ...)
{
    const char* logMessageHeader = nullptr;
    switch (logType)
    {
    case LogType::LOG_INFO:
    {
        logMessageHeader = "[INFO]: ";
        break;
    }
    case LogType::LOG_DEBUG:
    {
        logMessageHeader = "[DEBUG]: ";
        break;
    }
    case LogType::LOG_WARNING:
    {
        logMessageHeader = "[WARNING]: ";
        break;
    }
    case LogType::LOG_ERROR:
    {
        logMessageHeader = "[ERROR]: ";
        break;
    }
    default:
    {
        logMessageHeader = "[UNKNOWN]: ";
        break;
    }
    }
    char outputMessage[2048];
    std::memset(outputMessage, 0, sizeof(outputMessage));

    variable_arguments_list arguments;
    va_start(arguments, message);
    vsnprintf(outputMessage, 32000, message, arguments);
    va_end(arguments);

    std::string ttyMessage = std::format("{}{}\n", logMessageHeader, outputMessage);
    TTYOutput(ttyMessage.data());

#ifdef REMOTE_LOGGING
    samaritan::Logger::RedisLogger::GetInstance().Log(ttyMessage.data(), channel);
#endif
}
