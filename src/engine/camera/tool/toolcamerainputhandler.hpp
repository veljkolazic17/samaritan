#pragma once
#ifdef SM_TOOL
#include <defines.hpp>

#include <engine/events/eventhandlerwrapper.hpp>
#include <engine/input/events/keyboardevents.hpp>
#if HACKS_ENABLED
#include <engine/threads/iupdatable.hpp>
#endif
#if IMGUI_DISPLAY_ENABLED
#include <imgui/imguicentral.hpp>
#endif

#include <objecttemplates/singleton.hpp>
#include <map>

BEGIN_NAMESPACE

class ToolCameraInputHandler SINGLETON(ToolCameraInputHandler) HACK(, public IUpdatable) IMGUI_DISPLAY(, public IImguiModule)
{
public:
	SINGLETON_CONSTRUCTOR(ToolCameraInputHandler)
	void SingletonInit() override;
	HACK(void UpdateSingleThreaded();)
	HACK(void UpdateMultiThreaded() {};)

#if IMGUI_DISPLAY_ENABLED
	void DrawImgui() override;
#endif

private:
	void HandleOnKeyboardInputPressedEvent(const Input::KeyboardInputPressedEvent& event);
	Events::EventHandler<Input::KeyboardInputPressedEvent> m_KeyboardInputPressedEventHandler;

	void HandleOnKeyboardInputReleasedEvent(const Input::KeyboardInputReleasedEvent & event);
	Events::EventHandler<Input::KeyboardInputReleasedEvent> m_KeyboardInputReleasedEventHandler;


	smfloat32 m_CameraSpeed = 10.0f;
	std::map<Input::Key, smbool> m_KeyStates;
};

END_NAMESPACE
#endif