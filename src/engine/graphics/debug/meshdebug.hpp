#pragma once
#ifdef DEBUG
#include <defines.hpp>

#include <objecttemplates/singleton.hpp>
#include <engine/resources/graphics/mesh.hpp>
#include <engine/resources/resource.hpp>
#include <engine/events/eventhandlerwrapper.hpp>
#include <engine/input/events/keyboardevents.hpp>

#define MESH_DEBUG_KEY          ::samaritan::Input::Key::KEY_M
#define smMeshDebugSystem()     ::samaritan::MeshSystemDebug::GetInstance()

BEGIN_NAMESPACE

class MeshSystemDebug SINGLETON(MeshSystemDebug)
{
public:
    SINGLETON_CONSTRUCTOR(MeshSystemDebug)
    void SingletonInit() override;

    const Mesh* GetMesh() const { return m_Mesh.GetResource(); }

private:
    void HandleOnKeyboardInputPressedEvent(const Input::KeyboardInputPressedEvent& event);
    Events::EventHandler<Input::KeyboardInputPressedEvent> m_KeyboardInputPressedEventHandler;

    void HandleOnKeyboardInputReleasedEvent(const Input::KeyboardInputReleasedEvent& event);
    Events::EventHandler<Input::KeyboardInputReleasedEvent> m_KeyboardInputReleasedEventHandler;

    void LoadMesh(smcstring name);

    smcstring m_TestMeshes[3] =
    {
        "cube.gltf",
        "sphere.gltf",
        "adamHead.gltf"
    };

    ResourceHandle<Mesh> m_Mesh;
    smcstring m_LoadedMeshName = nullptr;

    smbool m_IsDebugKeyPressed = false;
};

END_NAMESPACE

#endif
