#ifdef DEBUG
#include <engine/events/eventmanager.hpp>
#include <engine/graphics/debug/geometrydebug.hpp>
#include <engine/graphics/debug/materialdebug.hpp>
#include <engine/graphics/renderer/frontend/rendererfrontend.hpp>
#include <engine/graphics/systems/materialsystem.hpp>
#include <utils/logger/log.hpp>

BEGIN_NAMESPACE

void GeometrySystemDebug::SingletonInit()
{
    GeometryConfig planeConfig1 = smGeometrySystem().GenerateGeometryConfig(
        10.0f, 10.0f, 10, 10, 1.0f, 1.0f,
        "PlaneDebug1", "rock");

    GeometryConfig planeConfig2 = smGeometrySystem().GenerateGeometryConfig(
        10.0f, 5.0f, 10, 10, 1.0f, 1.0f,
        "PlaneDebug2", "terrain");

    GeometryConfig planeConfig3 = smGeometrySystem().GenerateGeometryConfig(
        5.0, 10.0f, 10, 10, 1.0f, 1.0f,
        "PlaneDebug", "cliff");

    ::samaritan::GeometrySystem& geometrySystem = smGeometrySystem();

    m_GeometryConfigs[0] = planeConfig1;
    m_GeometryConfigs[1] = planeConfig2;
    m_GeometryConfigs[2] = planeConfig3;

    m_Geometries[0] = geometrySystem.AcquireFromConfig(planeConfig1);
    m_Geometries[1] = geometrySystem.AcquireFromConfig(planeConfig2);
    m_Geometries[2] = geometrySystem.AcquireFromConfig(planeConfig3);

    m_DebugGeometry = const_cast<Geometry*>(smGeometrySystem().GetDefaultGeometry());

    m_KeyboardInputPressedEventHandler = [this](const Input::KeyboardInputPressedEvent& event) { HandleOnKeyboardInputPressedEvent(event); };
    Events::Subscribe<Input::KeyboardInputPressedEvent>(m_KeyboardInputPressedEventHandler);

    m_KeyboardInputReleasedEventHandler = [this](const Input::KeyboardInputReleasedEvent& event) { HandleOnKeyboardInputReleasedEvent(event); };
    Events::Subscribe<Input::KeyboardInputReleasedEvent>(m_KeyboardInputReleasedEventHandler);
}

void GeometrySystemDebug::HandleOnKeyboardInputPressedEvent(const Input::KeyboardInputPressedEvent& event)
{
    LogDebug(LogChannel::Graphics, "Button pressed %s", Input::KeyToShortString(event.m_Key));

    smbool extraPressed = false;

    switch (event.m_Key)
    {
    case GEOMETRY_DEBUG_KEY:
    {
        m_IsDebugKeyPressed = true;
        break;
    }
    case Input::Key::KEY_F1:
    {
        m_DebugGeometry = m_Geometries[0];
        extraPressed = true;
        break;
    }
    case Input::Key::KEY_F2:
    {
        m_DebugGeometry = m_Geometries[1];
        extraPressed = true;
        break;
    }
    case Input::Key::KEY_F3:
    {
        m_DebugGeometry = m_Geometries[2];
        extraPressed = true;
        break;
    }
    default:
    {
        break;
    }
    }

    if (m_IsDebugKeyPressed && extraPressed)
    {
        smRenderer().SetGeometry(m_DebugGeometry);
        m_DebugGeometry->m_Material = smMaterialDebugSystem().GetMaterial();
    }
}

void GeometrySystemDebug::HandleOnKeyboardInputReleasedEvent(const Input::KeyboardInputReleasedEvent& event)
{
    if (event.m_Key == GEOMETRY_DEBUG_KEY)
    {
        m_IsDebugKeyPressed = false;
    }
}

GeometrySystemDebug::~GeometrySystemDebug()
{
    smGeometrySystem().Release(m_Geometries[0]);
    smGeometrySystem().Release(m_Geometries[1]);
    smGeometrySystem().Release(m_Geometries[2]);

    for (smuint8 i = 0; i < 3; ++i)
    {
        if (m_Geometries[i] != nullptr)
        {
            smGeometrySystem().DestroyGeometry(m_Geometries[i]);
            m_Geometries[i] = nullptr;
        }
        gpFreeArray(m_GeometryConfigs[i].m_Vertices, m_GeometryConfigs[i].m_VertexCount * sizeof(smVert3D));
        gpFreeArray(m_GeometryConfigs[i].m_Indices, m_GeometryConfigs[i].m_IndexCount * sizeof(smuint32));
    }
}

END_NAMESPACE
#endif
