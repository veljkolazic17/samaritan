#pragma once
#include <defines.hpp>

#include <engine/resources/graphics/mesh.hpp>

#include <objecttemplates/singleton.hpp>

#define smMeshSystem() ::samaritan::MeshSystem::GetInstance()

BEGIN_NAMESPACE

class MeshSystem SINGLETON(MeshSystem)
{
public:
    SINGLETON_CONSTRUCTOR(MeshSystem)

    void SingletonInit() override;
    void SingletonShutdown() override;

    smbool Init();
    void Shutdown();

    ResourceHandle<Mesh> Acquire(const smstring& path);
    void Release(const smstring& path);

    const Mesh* GetDefaultMesh() const { return m_DefaultMesh.get(); }

private:
    smbool CreateDefaultMesh();
    void DestroyMesh(Mesh* mesh);

    std::unique_ptr<Mesh> m_DefaultMesh = nullptr;
};

END_NAMESPACE