#ifdef REMOTE_LOGGING
#include <utils/logger/log.hpp>
#include <utils/logger/remotelogger/redislogger.hpp>

BEGIN_NAMESPACE

namespace Logger
{
    void RedisLogger::Start(const char* address, muint16 port)
    {
        m_RedisContext = redisConnect(address, port);
        if (m_RedisContext == nullptr || m_RedisContext->err) 
        {
            if (m_RedisContext) 
            {
                // Redis error
                LogErrorTTY("Redis connection error: %s", m_RedisContext->errstr);
                redisFree(m_RedisContext);
                m_RedisContext = nullptr;
            }
            else 
            {
                // Can't allocate redis context
                LogErrorTTY("Can't allocate redis context");
            }
        }
        else
        {
            LogInfoTTY("Connected with Redis on IP:%s PORT:%d", address, port);
            Publish(LogChannel::General, "Connection to Redis esatblished", m_RedisContext);
        }
    }

    RedisLogger::~RedisLogger()
    {
        if (m_RedisContext)
        {
            Publish(LogChannel::General, "Logger disconected", m_RedisContext);
            redisFree(m_RedisContext);
        }
    }

    void RedisLogger::Log(const char* message, LogChannel channel)
    {
        if (m_RedisContext)
        {
            Publish(LogChannel::General, message, m_RedisContext);
        }
    }
}

END_NAMESPACE

#endif