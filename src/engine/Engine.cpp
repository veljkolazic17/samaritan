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
#include <engine/graphics/systems/texturesystem.hpp>
#include <engine/graphics/systems/materialsystem.hpp>
#include <engine/graphics/systems/geometrysystem.hpp>
#include <engine/graphics/systems/shadersystem.hpp>
#if HACKS_ENABLED
#include <engine/threads/iupdatable.hpp>
#endif

#if IMGUI_DISPLAY_ENABLED
#include <imgui/imguidrawmodule.hpp>
#endif

#ifdef SM_TOOL
#include <engine/camera/tool/toolcamerainputhandler.hpp>
#endif

#ifdef DEBUG
#include <engine/graphics/debug/materialdebug.hpp>
#include <engine/graphics/debug/geometrydebug.hpp>
#endif

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

    void Engine::HandleOnWindowResizedEvent(const Graphics::WindowResizedEvent& event)
    {
        // TODO : Handle Minimization of window
        if (m_Window != nullptr)
        {
            m_Window->SetWidth(event.m_Width);
            m_Window->SetHeight(event.m_Height);
        }
        Graphics::Renderer::GetInstance().Resize(event.m_Width, event.m_Height);
    }

    void Engine::Init(void)
    {
#ifdef REMOTE_LOGGING
        samaritan::Logger::RedisLogger::GetInstance().Start("127.0.0.1", 6379);
#endif
        m_WindowResizedEventHandler = [this](const Graphics::WindowResizedEvent& event) { HandleOnWindowResizedEvent(event); };
        Events::Subscribe<Graphics::WindowResizedEvent>(m_WindowResizedEventHandler);

        Clock::GetInstance().Init();
        m_Window = Memory::mnew<Graphics::Window>(Memory::MemoryTag::MEM_RENDERER, 1280, 720, 100, 100, "My first win window :)");
        m_Window->Init();

        Events::EventManager::GetInstance().Init();
        Input::InputManager::GetInstance().Init();

        //This should be configurable
        Graphics::Renderer::GetInstance().Init(m_DeafualtRenderer);

        TextureSystem::GetInstance().Init();

        MaterialSystemConfig materialSystemConfig = { 4096 };
        MaterialSystem::GetInstance().Init(materialSystemConfig);

        GeometrySystemConfig geometryConfig = { 4096 };
        GeometrySystem::GetInstance().Init(geometryConfig);

        ShaderSystem::GetInstance().Init();
    }

    void Engine::Run(void)
    {
        Memory::SingleFrameAllocator& singleFrameAllocator = Memory::SingleFrameAllocator::GetInstance();
        Events::EventManager& eventManager = Events::EventManager::GetInstance();
        Input::InputManager& inputManager = Input::InputManager::GetInstance();
        Graphics::Renderer& renderer = Graphics::Renderer::GetInstance();
        Clock& clock = Clock::GetInstance();
#if HACKS_ENABLED
#ifdef SM_TOOL
        SM_INVOKE_SINGLETON_INIT(ToolCameraInputHandler)
#endif
#ifdef DEBUG
        SM_INVOKE_SINGLETON_INIT(MaterialSystemDebug);
        SM_INVOKE_SINGLETON_INIT(GeometrySystemDebug);
#endif
#endif

#if IMGUI_DISPLAY_ENABLED
        smImguiDrawModule().Init();
#endif
        clock.Start();
        ENGINE_RUN();
        // TODO : THIS CODE NEEDS TO BE REFACTORED WTF IS THIS
        while (IS_ENGINE_RUNNING())
        {
            clock.Update();
            m_DeltaTime = clock.GetElapsed() - m_LastLoopTime;
            singleFrameAllocator.Clear();
            LoopPreProcess();
            // Process Loop
            m_FrameTimeStart = clock.GetTime();
            Graphics::RenderData data;
            data.m_Time = m_DeltaTime;
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
            inputManager.Update(m_DeltaTime);

            m_LastLoopTime = clock.GetElapsed();
        }
    }

    void Engine::Shutdown(void)
    {
        ENGINE_SHUTDOWN();
        m_Window->Shutdown();
        Memory::mdelete<Graphics::Window>(m_Window);

        ShaderSystem::GetInstance().Shutdown();
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
#if HACKS_ENABLED
        UpdateSingleThreaded();
#endif
    }

#if HACKS_ENABLED
    void Engine::RegisterForSingleThreadedUpdate(IUpdatable* updatable)
    {
        m_SingleThreadedUpdatables.push_back(updatable);
    }

    void Engine::UpdateSingleThreaded()
    {
        for (IUpdatable* updatable : m_SingleThreadedUpdatables)
        {
            updatable->UpdateSingleThreaded();
        }
    }
#endif
}

END_NAMESPACE