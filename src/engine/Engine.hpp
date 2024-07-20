#pragma once
#include <defines.hpp>
#include <engine/graphics/window.hpp>
#include <engine/graphics/renderer/renderertype.inl>

// Why am I doing this???
#define ENGINE_RUN()			SetEngineState(EngineState::RUNNING);
#define ENGINE_SUSPEND()		SetEngineState(EngineState::SUSPENDED);
#define ENGINE_SHUTDOWN()		SetEngineState(EngineState::CLEANUP);

#define IS_ENGINE_RUNNING()		EngineState::RUNNING == GetEngineState()
#define IS_ENGINE_SUSPENDED()	EngineState::SUSPENDED == GetEngineState()
#define IS_ENGINE_SHUTDOWN()	EngineState::CLEANUP == GetEngineState()

BEGIN_NAMESPACE

namespace Engine
{
	enum class EngineState
	{
		PENDING,
		RUNNING,
		SUSPENDED,
		CLEANUP,
		SIZE = 4
	};

	class Engine
	{
	public:
		static Engine& GetInstance() { static Engine instance; return instance; }
		Engine(Engine& other) = delete;
		void operator=(const Engine&) = delete;
	private:
		Engine() {};
		~Engine() {};
	public:
		void Init(void);
		void Run(void);
		void Shutdown(void);

		inline EngineState GetEngineState(void) { return m_EngineState; }
		void SetEngineState(EngineState engineState);

		void LoopPreProcess(void);
		void LoopPostProcess(void);
	private:
		void LoopPreProcessPlatformImpl(void);

		EngineState m_EngineState = EngineState::PENDING;
		Graphics::Window* m_Window = nullptr;

		mfloat64 m_TargetFrameRate = 1.0 / 60.0;
		Time m_Runtime = 0;
		muint16 m_FrameCount = 0;
		Time m_FrameTimeStart = 0;
		Time m_FrameTimeEnd = 0;
		Time m_LastLoopTime = 0;

		mbool m_IsFrameLimitActive = false;

		Graphics::RendererType m_DeafualtRenderer = Graphics::RendererType::Vulkan;
	};

}

END_NAMESPACE