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

		void Update(mfloat64 deltaTime);

		//Keyboard
		void ProcessKey(Key key, mbool isPressed);
		mbool IsKeyUp(Key key);
		mbool IsKeyDown(Key key);
		mbool WasKeyUp(Key key);
		mbool WasKeyDown(Key key);
		
		//Mouse
		void ProcessMouseButton(MouseButton button, mbool isPressed);
		void ProcessMouseMovement(mint16 x, mint16 y);
		void ProcessMouseWheel(mint16 deltaMove);
		mbool IsMouseButtonUp(MouseButton button);
		mbool IsMouseButtonDown(MouseButton button);
		mbool WasMouseButtonUp(MouseButton button);
		mbool WasMouseButtonDown(MouseButton button);

		//XBox

	private:
		//TODO : Only supports one player
		InputState m_InputStates;
		mbool m_IsInitialized = false;
	};
}

END_NAMESPACE