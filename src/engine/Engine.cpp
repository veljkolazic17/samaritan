#include <engine/Engine.hpp>
#include <utils/asserts/assert.hpp>
#include <utils/logger/remotelogger/redislogger.hpp>
#include <engine/clock.hpp>
#include <engine/memory/memory.hpp>
//TODO : this should be changed to be in another folder
#include <engine/memory/containers/singleframeallocator.hpp>

BEGIN_NAMESPACE

namespace Engine
{
	void Engine::SetEngineState(EngineState engineState)
	{
		switch(m_EngineState)
		{ 
			case EngineState::PENDING :
			{
				if (engineState != EngineState::RUNNING)
				{
					hardAssert(false, "Engine not running!");
				}
			}
			break;
			case EngineState::RUNNING :
			case EngineState::SUSPENDED :
			{
				if (engineState == EngineState::PENDING)
				{
					hardAssert(false, "Engine is running!");
				}
			}
			break;
			case EngineState::CLEANUP :
			{
				hardAssert(false, "Engine is in cleanup phase!");	
			}
			break;
		}
		m_EngineState = engineState;
	}

	void Engine::Init(void)
	{
#ifdef REMOTE_LOGGING
		samaritan::Logger::RedisLogger::GetInstance().Start("127.0.0.1", 6379);
#endif
		Clock::GetInstance().Init();
		// TODO : Not good way of doing this ! Don't use copy constructor
		m_Window = Memory::mnew<Graphics::Window>(1280, 720, 100, 100, "My first win window :)");
		m_Window->Init();
	}

	void Engine::Run(void)
	{
		Memory::SingleFrameAllocator& singleFrameAllocator = Memory::SingleFrameAllocator::GetInstance();

		ENGINE_RUN();
		while (IS_ENGINE_RUNNING())
		{
			LoopPreProcess();
			singleFrameAllocator.Clear();
			// Process Loop
		}
	}

	void Engine::Shutdown(void)
	{
		ENGINE_SHUTDOWN();
		m_Window->Shutdown();
		Memory::mdelete<Graphics::Window>(m_Window);
	}

	void Engine::LoopPreProcess(void)
	{
		LoopPreProcessPlatformImpl();
	}

}

END_NAMESPACE