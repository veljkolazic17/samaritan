#ifdef DEBUG
#include <engine/graphics/debug/meshdebug.hpp>

#include <engine/events/eventmanager.hpp>
#include <engine/graphics/renderer/frontend/rendererfrontend.hpp>
#include <engine/graphics/systems/meshsystem.hpp>
#include <engine/input/input.hpp>
#include <utils/logger/log.hpp>

BEGIN_NAMESPACE

void MeshSystemDebug::SingletonInit()
{
    m_KeyboardInputPressedEventHandler = [this](const Input::KeyboardInputPressedEvent& event) { HandleOnKeyboardInputPressedEvent(event); };
    Events::Subscribe<Input::KeyboardInputPressedEvent>(m_KeyboardInputPressedEventHandler);

    m_KeyboardInputReleasedEventHandler = [this](const Input::KeyboardInputReleasedEvent& event) { HandleOnKeyboardInputReleasedEvent(event); };
    Events::Subscribe<Input::KeyboardInputReleasedEvent>(m_KeyboardInputReleasedEventHandler);
}

void MeshSystemDebug::HandleOnKeyboardInputPressedEvent(const Input::KeyboardInputPressedEvent& event)
{
    LogDebug(LogChannel::Graphics, "Button pressed %s", Input::KeyToShortString(event.m_Key));

    smcstring meshName = nullptr;

    switch (event.m_Key)
    {
    case MESH_DEBUG_KEY:
    {
        m_IsDebugKeyPressed = true;
        break;
    }
    case Input::Key::KEY_F1:
    {
        meshName = m_TestMeshes[0];
        break;
    }
    case Input::Key::KEY_F2:
    {
        meshName = m_TestMeshes[1];
        break;
    }
    case Input::Key::KEY_F3:
    {
        meshName = m_TestMeshes[2];
        break;
    }
    default:
        break;
    }

    if (m_IsDebugKeyPressed && meshName != nullptr)
    {
        LoadMesh(meshName);
    }
}

void MeshSystemDebug::HandleOnKeyboardInputReleasedEvent(const Input::KeyboardInputReleasedEvent& event)
{
    if (event.m_Key == MESH_DEBUG_KEY)
    {
        m_IsDebugKeyPressed = false;
    }
}

void MeshSystemDebug::LoadMesh(smcstring name)
{
    ResourceHandle<Mesh> handle = smMeshSystem().Acquire(name);
    if (!handle.IsValid())
    {
        LogError(LogChannel::Graphics, "MeshDebug: failed to load mesh '%s'", name);
        return;
    }

    if (m_LoadedMeshName != nullptr)
    {
        smMeshSystem().Release(m_LoadedMeshName);
    }

    m_Mesh = handle;
    m_LoadedMeshName = name;

    const Mesh* mesh = m_Mesh.GetResource();
    smRenderer().SetMesh(mesh);

    LogDebug(LogChannel::Graphics, "MeshDebug: loaded '%s' (%zu geometries)", name, mesh->m_Geometries.size());
}

END_NAMESPACE
#endif
