#include <engine/graphics/systems/meshsystem.hpp>

#include <engine/graphics/systems/geometrysystem.hpp>
#include <engine/graphics/systems/materialsystem.hpp>
#include <engine/resources/resourcesystem.hpp>

#include <utils/logger/log.hpp>

BEGIN_NAMESPACE

smbool MeshSystem::Init()
{
    if (!CreateDefaultMesh())
    {
        softAssert(false, "Failed to create default mesh!");
        return false;
    }
    return true;
}

void MeshSystem::Shutdown()
{
    if (m_DefaultMesh)
    {
        DestroyMesh(m_DefaultMesh.get());
        m_DefaultMesh = nullptr;
    }
}

void MeshSystem::SingletonInit()
{
    Init();
}

void MeshSystem::SingletonShutdown()
{
    Shutdown();
}

ResourceHandle<Mesh> MeshSystem::Acquire(const smstring& path)
{
    if (path == SM_DEFAULT_MESH_NAME)
    {
        return smResourceSystem().Load<Mesh>(m_DefaultMesh.get(), false);
    }
    return smResourceSystem().Load<Mesh>(path);
}

void MeshSystem::Release(const smstring& path)
{
    if (path == SM_DEFAULT_MESH_NAME)
    {
        softAssert(false, "Trying to release default mesh!");
        return;
    }
    smResourceSystem().ForceUnload(path);
}

smbool MeshSystem::CreateDefaultMesh()
{
    GeometryConfig config = smGeometrySystem().GenerateCubeGeometryConfig(1.0f, SM_DEFAULT_MESH_NAME, SM_DEFAULT_MATERIAL_NAME);
    Geometry* geometry = smGeometrySystem().AcquireFromConfig(config);
    if (geometry == nullptr)
    {
        softAssert(false, "Failed to create default mesh geometry!");
        return false;
    }

    m_DefaultMesh = std::make_unique<Mesh>(SM_DEFAULT_MESH_NAME);
    m_DefaultMesh->m_Geometries.push_back(geometry);
    m_DefaultMesh->m_State = ResourceState::Loaded;
    return true;
}

void MeshSystem::DestroyMesh(Mesh* mesh)
{
    if (mesh == nullptr)
    {
        softAssert(false, "Trying to destroy invalid mesh!");
        return;
    }

    for (Geometry* geometry : mesh->m_Geometries)
    {
        smGeometrySystem().Release(geometry);
    }
    mesh->m_Geometries.clear();
}

END_NAMESPACE
