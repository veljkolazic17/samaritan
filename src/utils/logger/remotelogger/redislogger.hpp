#ifdef REMOTE_LOGGING
#include <defines.hpp>
#include <hiredis/hiredis.h>
#include <utils/logger/logchannels.hpp>

#define SendCommand(command, channel, message, context) redisAppendCommand(context, "%s %s %s", #command, LogChannelToString(channel), message);

#define Publish(channel, message, context) SendCommand(PUBLISH, channel, message, context)

BEGIN_NAMESPACE

namespace Logger
{
    class RedisLogger
    {
    public:
        static RedisLogger& GetInstance() { static RedisLogger instance; return instance; }
        RedisLogger(RedisLogger& other) = delete;
        void operator=(const RedisLogger&) = delete;
    private:
        RedisLogger() {};
        ~RedisLogger();

    public:
        void Start(const char* address, smuint16 port);

        void Log(const char* message, LogChannel channel);
    private:
        smbool m_IsLoggerRunning = false;
        redisContext* m_RedisContext = nullptr;
    };
}

END_NAMESPACE

#endif