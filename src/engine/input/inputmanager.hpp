#pragma once
#include <defines.hpp>
#include <objecttemplates/singleton.hpp>
#include <objecttemplates/shutdownable.hpp>

#include <engine/input/inputstate.hpp>

BEGIN_NAMESPACE

namespace Input
{
	class InputManager SINGLETON(InputManager), public Shutdownable
	{
	private:
		SINGLETON_CONSTRUCTOR(InputManager)
	public:
		void Init() override;
		void Shutdown() override;
		void SingletonInit() override;
		void SingletonShutdown() override;

		void Update(smfloat64 deltaTime);

		//Keyboard
		void ProcessKey(Key key, smbool isPressed);
		smbool IsKeyUp(Key key);
		smbool IsKeyDown(Key key);
		smbool WasKeyUp(Key key);
		smbool WasKeyDown(Key key);
		
		//Mouse
		void ProcessMouseButton(MouseButton button, smbool isPressed);
		void ProcessMouseMovement(smint16 x, smint16 y);
		void ProcessMouseWheel(smint16 deltaMove);
		smbool IsMouseButtonUp(MouseButton button);
		smbool IsMouseButtonDown(MouseButton button);
		smbool WasMouseButtonUp(MouseButton button);
		smbool WasMouseButtonDown(MouseButton button);

		//XBox

	private:
		//TODO : Only supports one player
		InputState m_InputStates;
		smbool m_IsInitialized = false;
	};
}

END_NAMESPACE