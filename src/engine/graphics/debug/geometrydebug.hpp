#pragma once
#ifdef DEBUG
#include <defines.hpp>

#include <objecttemplates/singleton.hpp>
#include <engine/graphics/systems/geometrysystem.hpp>
#include <engine/resources/graphics/geometry.hpp>
#include <engine/events/eventhandlerwrapper.hpp>
#include <engine/input/events/keyboardevents.hpp>

#define GEOMETRY_DEBUG_KEY    ::samaritan::Input::Key::KEY_G
#define smGeometryDebugSystem()     ::samaritan::GeometrySystemDebug::GetInstance()

BEGIN_NAMESPACE

class GeometrySystemDebug SINGLETON(GeometrySystemDebug)
{
public:
    SINGLETON_CONSTRUCTOR(GeometrySystemDebug)
    void SingletonInit() override;

    ~GeometrySystemDebug();

    SM_INLINE Geometry* GetGeometry() { return m_DebugGeometry; }
private:
    void HandleOnKeyboardInputPressedEvent(const Input::KeyboardInputPressedEvent& event);
    Events::EventHandler<Input::KeyboardInputPressedEvent> m_KeyboardInputPressedEventHandler;

    void HandleOnKeyboardInputReleasedEvent(const Input::KeyboardInputReleasedEvent& event);
    Events::EventHandler<Input::KeyboardInputReleasedEvent> m_KeyboardInputReleasedEventHandler;

    Geometry* m_DebugGeometry;

    smbool m_IsDebugKeyPressed = false;

    Geometry* m_Geometries[3] =
    {
        nullptr,
        nullptr,
        nullptr
    };

    GeometryConfig m_GeometryConfigs[3];
};

END_NAMESPACE

#endif