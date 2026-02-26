#pragma once
#ifdef DEBUG
#include <defines.hpp>

#include <objecttemplates/singleton.hpp>
#include <engine/events/eventhandlerwrapper.hpp>
#include <engine/input/events/keyboardevents.hpp>

#define TEXTURE_DEBUG_KEY	::samaritan::Input::Key::KEY_T
#define smMaterialDebugSystem()     ::samaritan::MaterialSystemDebug::GetInstance()

BEGIN_NAMESPACE

class MaterialSystemDebug SINGLETON(MaterialSystemDebug)
{
public:
	SINGLETON_CONSTRUCTOR(MaterialSystemDebug)
	void SingletonInit() override;

    const Material* GetMaterial() const { return m_Material; }
    Material* GetMaterial() { return m_Material; }

private:
	void HandleOnKeyboardInputPressedEvent(const Input::KeyboardInputPressedEvent & event);
	Events::EventHandler<Input::KeyboardInputPressedEvent> m_KeyboardInputPressedEventHandler;

	void HandleOnKeyboardInputReleasedEvent(const Input::KeyboardInputReleasedEvent & event);
	Events::EventHandler<Input::KeyboardInputReleasedEvent> m_KeyboardInputReleasedEventHandler;

	smcstring m_TestTextures[3] = 
	{
		"terrain",
		"cliff",
		"rock"
	};

	smcstring m_LoadedImage = nullptr;

	Material* m_Material = nullptr;

	smbool m_IsDebugKeyPressed = false;
};

END_NAMESPACE

#endif