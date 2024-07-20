#include <engine/Engine.hpp>
#include <utils/asserts/assert.hpp>
#include <utils/logger/remotelogger/redislogger.hpp>
#include <engine/clock.hpp>
#include <engine/memory/memory.hpp>
#include <engine/events/eventmanager.hpp>
#include <engine/input/inputmanager.hpp>
#include <engine/graphics/renderer/frontend/rendererfrontend.hpp>
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
		// What theee fuuuuuck is this?
		m_Window = Memory::mnew<Graphics::Window>(Memory::MemoryTag::MEM_RENDERER, 1280, 720, 100, 100, "My first win window :)");
		m_Window->Init();

		Events::EventManager::GetInstance().Init();
		Input::InputManager::GetInstance().Init();

		//This should be configurable
		Graphics::Renderer::GetInstance().Init(m_DeafualtRenderer);
	}

	void Engine::Run(void)
	{
		Memory::SingleFrameAllocator& singleFrameAllocator = Memory::SingleFrameAllocator::GetInstance();
		Events::EventManager& eventManager = Events::EventManager::GetInstance();
		Input::InputManager& inputManager = Input::InputManager::GetInstance();
		Graphics::Renderer& renderer = Graphics::Renderer::GetInstance();
		Clock& clock = Clock::GetInstance();


		clock.Start();
		clock.Update();
		ENGINE_RUN();
		while (IS_ENGINE_RUNNING())
		{
			// TODO : THIS CODE NEEDS TO BE REFACTORED WTF IS THIS
			singleFrameAllocator.Clear();
			LoopPreProcess();
			// Process Loop
			Time deltaTime = clock.GetElapsed() - m_LastLoopTime;
			m_FrameTimeStart = clock.GetTime();
			Graphics::RenderData data;
			data.m_Time = deltaTime;
			renderer.DrawFrame(data);
			//End of loop
			m_FrameTimeEnd = clock.GetTime();
			Time frameElapsedTime = m_FrameTimeEnd - m_FrameTimeStart;
			m_Runtime += frameElapsedTime;
			Time remainingTargetTimeSeconds = m_TargetFrameRate - frameElapsedTime;

			if (remainingTargetTimeSeconds > 0)
			{
				if (!m_IsFrameLimitActive)
				{
					clock.Sleep(remainingTargetTimeSeconds * 1000 - 1);
				}
				++m_FrameCount;
			}

			LoopPostProcess();
			eventManager.DispatchEvents();
			inputManager.Update(0);

			m_LastLoopTime = clock.GetElapsed();
		}
	}

	void Engine::Shutdown(void)
	{
		ENGINE_SHUTDOWN();
		m_Window->Shutdown();
		Memory::mdelete<Graphics::Window>(m_Window);

		Input::InputManager::GetInstance().Shutdown();
		Events::EventManager::GetInstance().Shutdown();

		Graphics::Renderer::GetInstance().Shutdown();
	}

	void Engine::LoopPreProcess(void)
	{
		LoopPreProcessPlatformImpl();
	}

	void Engine::LoopPostProcess(void)
	{

	}

}

END_NAMESPACE