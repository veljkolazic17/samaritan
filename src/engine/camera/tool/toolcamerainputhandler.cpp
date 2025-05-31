#ifdef SM_TOOL
#include <engine/camera/tool/toolcamerainputhandler.hpp>

#include <engine/camera/camera.hpp>
#include <engine/events/eventmanager.hpp>
#include <engine/graphics/renderer/frontend/rendererfrontend.hpp>

#if HACKS_ENABLED
#include <engine/Engine.hpp>
#endif

BEGIN_NAMESPACE

void ToolCameraInputHandler::SingletonInit()
{
	m_KeyboardInputPressedEventHandler = [this](const Input::KeyboardInputPressedEvent& event) { HandleOnKeyboardInputPressedEvent(event); };
	Events::Subscribe<Input::KeyboardInputPressedEvent>(m_KeyboardInputPressedEventHandler);

	m_KeyboardInputReleasedEventHandler = [this](const Input::KeyboardInputReleasedEvent& event) { HandleOnKeyboardInputReleasedEvent(event); };
	Events::Subscribe<Input::KeyboardInputReleasedEvent>(m_KeyboardInputReleasedEventHandler);

	HACK(smEngine().RegisterForSingleThreadedUpdate(this);)
}

void ToolCameraInputHandler::HandleOnKeyboardInputReleasedEvent(const Input::KeyboardInputReleasedEvent& event)
{
	m_KeyStates[event.m_Key] = false;
}

void ToolCameraInputHandler::HandleOnKeyboardInputPressedEvent(const Input::KeyboardInputPressedEvent& event)
{
	m_KeyStates[event.m_Key] = true;
}

#if HACKS_ENABLED
void ToolCameraInputHandler::UpdateSingleThreaded()
{
	constexpr mfloat32 moveSpeed = 40.0f;
	smVec3 moveVec = smVec3_zero;
	Time deltaTime = smEngine().GetDeltaTime();

	Camera& camera = smCamera();

	Graphics::Renderer& renderer = smRenderer();

	mbool needUpdate = false;

	for (const auto& [key, isPressed] : m_KeyStates)
	{
		if (isPressed)
		{
            needUpdate = true;

			switch (key)
			{
			case Input::Key::KEY_W:
			{
				moveVec += renderer.GetView().Forward();
				break;
			}
			case Input::Key::KEY_S:
			{
				moveVec += renderer.GetView().Backward();
				break;
			}
			case Input::Key::KEY_D:
			{
				moveVec += renderer.GetView().Right();
				break;
			}
			case Input::Key::KEY_A:
			{
				moveVec += renderer.GetView().Left();
				break;
			}
			case Input::Key::KEY_UP:
			{
				camera.ChangePitch(deltaTime);
				break;
			}
			case Input::Key::KEY_DOWN:
			{
				camera.ChangePitch(-1.0f * deltaTime);
				break;
			}
			case Input::Key::KEY_LEFT:
			{
				camera.ChangeYaw(deltaTime);
				break;
			}
			case Input::Key::KEY_RIGHT:
			{
				camera.ChangeYaw(-1.0f * deltaTime);
				break;
			}
			}
		}
	}

    if (needUpdate)
    {
		moveVec.Normalize();
		smVec3& cameraPosition = camera.GetCameraPosition();
		mfloat32 move = deltaTime * moveSpeed;
		cameraPosition.m_X += moveVec.m_X * move;
		cameraPosition.m_Y += moveVec.m_Y * move;
		cameraPosition.m_Z += moveVec.m_Z * move;
		camera.SetIsViewDirty(true);

		camera.CalculateNewView();
		renderer.SetView(camera.GetCameraView());
    }
}
#endif

END_NAMESPACE
#endif