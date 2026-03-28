#include <editor/tools/entitypicker.hpp>

#ifdef SM_TOOL
#include <engine/events/eventmanager.hpp>
#include <engine/graphics/renderer/frontend/rendererfrontend.hpp>
#include <engine/input/inputmanager.hpp>

#include <utils/logger/log.hpp>

BEGIN_NAMESPACE

void EntityPicker::SingletonInit()
{
    m_MouseButtonPressedHandler = [this](const Input::MouseButtonPressedEvent& event) { HandleOnMouseButtonPressed(event); };
    Events::Subscribe<Input::MouseButtonPressedEvent>(m_MouseButtonPressedHandler);
}

void EntityPicker::SingletonShutdown()
{
}

void EntityPicker::HandleOnMouseButtonPressed(const Input::MouseButtonPressedEvent& event)
{
    if (event.m_Button == Input::MouseButton::MOUSE_LEFT)
    {
        m_PickRequested = true;
        m_ClickX = Input::InputManager::GetInstance().GetMouseX();
        m_ClickY = Input::InputManager::GetInstance().GetMouseY();
    }

    if (m_PickRequested)
    {
        m_PickRequested = false;
        m_PickedObjectId = smRenderer().PickEntity(m_ClickX, m_ClickY);
    }
}

END_NAMESPACE
#endif