#pragma once
#ifdef SM_TOOL
#include <defines.hpp>

#include <objecttemplates/singleton.hpp>
#include <engine/events/eventhandlerwrapper.hpp>
#include <engine/input/events/keyboardevents.hpp>

BEGIN_NAMESPACE

class ToolCameraInputHandler SINGLETON(ToolCameraInputHandler)
{
public:
	SINGLETON_CONSTRUCTOR_INIT(ToolCameraInputHandler)
	void SingletonInit() override;
private:
	void HandleOnKeyboardInputPressedEvent(const Input::KeyboardInputPressedEvent& event);
	Events::EventHandler<Input::KeyboardInputPressedEvent> m_KeyboardInputPressedEventHandler;
};

END_NAMESPACE
#endif