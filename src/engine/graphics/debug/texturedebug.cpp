#ifdef DEBUG
#include <engine/graphics/debug/texturedebug.hpp>
#include <engine/events/eventmanager.hpp>
#include <engine/graphics/systems/texturesystem.hpp>
#ifdef TEST_CODE_ENABLED
#include <engine/graphics/renderer/frontend/rendererfrontend.hpp>
#endif
BEGIN_NAMESPACE

void TextureSystemDebug::SingletonInit()
{
	m_KeyboardInputPressedEventHandler = [this](const Input::KeyboardInputPressedEvent& event) { HandleOnKeyboardInputPressedEvent(event); };
	Events::Subscribe<Input::KeyboardInputPressedEvent>(m_KeyboardInputPressedEventHandler);

	m_KeyboardInputReleasedEventHandler = [this](const Input::KeyboardInputReleasedEvent& event) { HandleOnKeyboardInputReleasedEvent(event); };
	Events::Subscribe<Input::KeyboardInputReleasedEvent>(m_KeyboardInputReleasedEventHandler);
}

void TextureSystemDebug::HandleOnKeyboardInputPressedEvent(const Input::KeyboardInputPressedEvent& event)
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
		if (Texture* texture = smTextureSystem().Acquire(testTextureName, true))
		{
#ifdef TEST_CODE_ENABLED
			smRenderer().SetTestTexture(texture);
#endif
			if (m_LoadedImage != nullptr)
			{
				smTextureSystem().Release(m_LoadedImage);
			}
			m_LoadedImage = testTextureName;
		}
	}
}

void TextureSystemDebug::HandleOnKeyboardInputReleasedEvent(const Input::KeyboardInputReleasedEvent& event)
{
	if (event.m_Key == TEXTURE_DEBUG_KEY)
	{
		m_IsDebugKeyPressed = false;
	}
}

END_NAMESPACE
#endif
