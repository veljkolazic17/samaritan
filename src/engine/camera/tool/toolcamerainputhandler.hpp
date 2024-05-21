#pragma once
#ifdef SM_TOOL
#include <defines.hpp>

#include <engine/events/eventhandlerwrapper.hpp>
#include <engine/input/events/keyboardevents.hpp>
#if HACKS_ENABLED
#include <engine/threads/iupdatable.hpp>
#endif

#include <objecttemplates/singleton.hpp>
#include <map>

BEGIN_NAMESPACE

class ToolCameraInputHandler SINGLETON(ToolCameraInputHandler) HACK(, public IUpdatable)
{
public:
	SINGLETON_CONSTRUCTOR_INIT(ToolCameraInputHandler)
	void SingletonInit() override;
	HACK(void UpdateSingleThreaded();)
	HACK(void UpdateMultiThreaded() {};)
private:
	void HandleOnKeyboardInputPressedEvent(const Input::KeyboardInputPressedEvent& event);
	Events::EventHandler<Input::KeyboardInputPressedEvent> m_KeyboardInputPressedEventHandler;

	void HandleOnKeyboardInputReleasedEvent(const Input::KeyboardInputReleasedEvent & event);
	Events::EventHandler<Input::KeyboardInputReleasedEvent> m_KeyboardInputReleasedEventHandler;

	std::map<Input::Key, smbool> m_KeyStates;
};

END_NAMESPACE
#endif