#ifdef DEBUG
#include <engine/graphics/debug/materialdebug.hpp>
#include <engine/events/eventmanager.hpp>
#include <engine/graphics/systems/materialsystem.hpp>
#ifdef TEST_CODE_ENABLED
#include <engine/graphics/renderer/frontend/rendererfrontend.hpp>
#endif
BEGIN_NAMESPACE

void MaterialSystemDebug::SingletonInit()
{
	m_KeyboardInputPressedEventHandler = [this](const Input::KeyboardInputPressedEvent& event) { HandleOnKeyboardInputPressedEvent(event); };
	Events::Subscribe<Input::KeyboardInputPressedEvent>(m_KeyboardInputPressedEventHandler);

	m_KeyboardInputReleasedEventHandler = [this](const Input::KeyboardInputReleasedEvent& event) { HandleOnKeyboardInputReleasedEvent(event); };
	Events::Subscribe<Input::KeyboardInputReleasedEvent>(m_KeyboardInputReleasedEventHandler);
}

void MaterialSystemDebug::HandleOnKeyboardInputPressedEvent(const Input::KeyboardInputPressedEvent& event)
{
	mcstring testTextureName = nullptr;

	switch (event.m_Key)
	{
	case TEXTURE_DEBUG_KEY:
	{
		m_IsDebugKeyPressed = true;
		break;
	}
	case Input::Key::KEY_F1:
	{
		testTextureName = m_TestTextures[0];
		break;
	}
	case Input::Key::KEY_F2:
	{
		testTextureName = m_TestTextures[1];
		break;
	}
	case Input::Key::KEY_F3:
	{
		testTextureName = m_TestTextures[2];
		break;
	}
	default:
	{
		break;
	}
	}

	if (m_IsDebugKeyPressed && testTextureName != nullptr)
	{
		if (Material* material = smMaterialSystem().Acquire(testTextureName))
		{
			smRenderer().SetTestMaterial(material);

			if (m_LoadedImage != nullptr)
			{
				smMaterialSystem().Release(m_LoadedImage);
			}
			m_LoadedImage = testTextureName;
		}
	}
}

void MaterialSystemDebug::HandleOnKeyboardInputReleasedEvent(const Input::KeyboardInputReleasedEvent& event)
{
	if (event.m_Key == TEXTURE_DEBUG_KEY)
	{
		m_IsDebugKeyPressed = false;
	}
}

END_NAMESPACE
#endif
