#pragma once
#include <defines.hpp>
#include <engine/graphics/window.hpp>
#include <engine/graphics/renderer/renderertype.inl>
#include <engine/events/eventhandlerwrapper.hpp>
#include <engine/graphics/events/windowevents.hpp>
#include <objecttemplates/singleton.hpp>
#include <objecttemplates/shutdownable.hpp>
#include <vector>

// Why am I doing this???
#define ENGINE_RUN()			SetEngineState(EngineState::RUNNING);
#define ENGINE_SUSPEND()		SetEngineState(EngineState::SUSPENDED);
#define ENGINE_SHUTDOWN()		SetEngineState(EngineState::CLEANUP);

#define IS_ENGINE_RUNNING()		EngineState::RUNNING == GetEngineState()
#define IS_ENGINE_SUSPENDED()	EngineState::SUSPENDED == GetEngineState()
#define IS_ENGINE_SHUTDOWN()	EngineState::CLEANUP == GetEngineState()

#define smEngine()				::samaritan::Engine::Engine::GetInstance()

BEGIN_NAMESPACE

class IUpdatable;

namespace Engine
{
	enum class EngineState
	{
		PENDING,
		RUNNING,
		SUSPENDED,
		CLEANUP,
		SIZE
	};

	class Engine SINGLETON(Engine), public Shutdownable
	{
	private:
		SINGLETON_CONSTRUCTOR(Engine);
	public:
		void Init(void) override;
		void BigInit(void);
		void BigShutdown(void);
		void Shutdown(void) override;
		void Run(void);

		SM_INLINE EngineState GetEngineState(void) { return m_EngineState; }
		void SetEngineState(EngineState engineState);

		SM_INLINE Time GetDeltaTime() { return m_DeltaTime; }

		void LoopPreProcess(void);
		void LoopPostProcess(void);

		SM_INLINE const Graphics::Window* const GetWindow() { return m_Window; }
#if HACKS_ENABLED
		void RegisterForSingleThreadedUpdate(IUpdatable* updatable);
#endif
	private:
		void LoopPreProcessPlatformImpl(void);

#if HACKS_ENABLED
		void UpdateSingleThreaded();
		std::vector<IUpdatable*> m_SingleThreadedUpdatables;
#endif

		EngineState m_EngineState = EngineState::PENDING;
		Graphics::Window* m_Window = nullptr;

		smfloat64 m_TargetFrameRate = 1.0 / 60.0;
		Time m_Runtime = 0;
		smuint16 m_FrameCount = 0;
		Time m_FrameTimeStart = 0;
		Time m_FrameTimeEnd = 0;
		Time m_LastLoopTime = 0;
		Time m_DeltaTime = 0;

		smbool m_IsFrameLimitActive = false;

		Graphics::RendererType m_DeafualtRenderer = Graphics::RendererType::Vulkan;

		void HandleOnWindowResizedEvent(const Graphics::WindowResizedEvent& event);
		Events::EventHandler<Graphics::WindowResizedEvent> m_WindowResizedEventHandler;
	};

}

END_NAMESPACE