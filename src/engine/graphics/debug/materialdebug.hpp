#pragma once
#ifdef DEBUG
#include <defines.hpp>

#include <objecttemplates/singleton.hpp>
#include <engine/events/eventhandlerwrapper.hpp>
#include <engine/input/events/keyboardevents.hpp>

#define TEXTURE_DEBUG_KEY	::samaritan::Input::Key::KEY_T

BEGIN_NAMESPACE

class MaterialSystemDebug SINGLETON(MaterialSystemDebug)
{
public:
	SINGLETON_CONSTRUCTOR_INIT(MaterialSystemDebug)
	void SingletonInit() override;

private:
	void HandleOnKeyboardInputPressedEvent(const Input::KeyboardInputPressedEvent & event);
	Events::EventHandler<Input::KeyboardInputPressedEvent> m_KeyboardInputPressedEventHandler;

	void HandleOnKeyboardInputReleasedEvent(const Input::KeyboardInputReleasedEvent & event);
	Events::EventHandler<Input::KeyboardInputReleasedEvent> m_KeyboardInputReleasedEventHandler;

	mcstring m_TestTextures[3] = 
	{
		"terrain",
		"cliff",
		"rock"
	};

	mcstring m_LoadedImage = nullptr;

	mbool m_IsDebugKeyPressed = false;
};

END_NAMESPACE

#endif