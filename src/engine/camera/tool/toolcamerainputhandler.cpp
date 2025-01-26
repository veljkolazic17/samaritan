#include "engine/Engine.hpp"
#ifdef SM_TOOL
#include <engine/camera/tool/toolcamerainputhandler.hpp>

#include <engine/camera/camera.hpp>
#include <engine/events/eventmanager.hpp>
#include <engine/graphics/renderer/frontend/rendererfrontend.hpp>


BEGIN_NAMESPACE

void ToolCameraInputHandler::SingletonInit()
{
	m_KeyboardInputPressedEventHandler = [this](const Input::KeyboardInputPressedEvent& event) { HandleOnKeyboardInputPressedEvent(event); };
	Events::Subscribe<Input::KeyboardInputPressedEvent>(m_KeyboardInputPressedEventHandler);
}

void ToolCameraInputHandler::HandleOnKeyboardInputPressedEvent(const Input::KeyboardInputPressedEvent& event)
{
	constexpr mfloat32 moveSpeed = 40.0f;
	smVec3 moveVec = smVec3_zero;
	Time deltaTime = smEngine().GetDeltaTime();

	Camera& camera = smCamera();

	Graphics::Renderer& renderer = smRenderer();

	switch (event.m_Key)
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
	default:
	{
		return;
	}
	}

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


END_NAMESPACE
#endif
