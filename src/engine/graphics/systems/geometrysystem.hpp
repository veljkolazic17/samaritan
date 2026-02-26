#pragma once
#include <defines.hpp>

#include <engine/resources/graphics/geometry.hpp>

#include <objecttemplates/singleton.hpp>
#include <math/vector.hpp>
#include <vector>

BEGIN_NAMESPACE

#define smGeometrySystem()          ::samaritan::GeometrySystem::GetInstance()

//TODO : [GRAPHICS] Make reference system for whole engine
struct GeometryReference
{
    smuint64 m_RefCount = 0;
    Geometry m_Geometry;
    smbool m_ShouldAutoRelease = false;
};

struct GeometrySystemConfig
{
    smuint32 m_MaxGeometryCount;
};

struct GeometryConfig
{
    smbool m_ShouldAutoRelease = false;
    smuint32 m_VertexCount = 0;
    smuint32 m_IndexCount = 0;
    smVert3D* m_Vertices = nullptr;
    smuint32* m_Indices = nullptr;
    smuint8 m_GeometryName[SM_GEOMETRY_NAME_MAX_LENGTH];
    smuint8 m_MaterialName[SM_GEOMETRY_NAME_MAX_LENGTH];
};

class GeometrySystem SINGLETON(GeometrySystem)
{
public:
    SINGLETON_CONSTRUCTOR(GeometrySystem)

    smbool Init(const GeometrySystemConfig & config);
    void Shutdown();
    void SingletonInit() override;
    void SingletonShutdown() override;

    Geometry* AcquireById(smuint32 id);
    void Release(Geometry* geometry);
    const Geometry* GetDefaultGeometry() { return m_DefaultGeometry; }
    smbool CreateDefaultGeometry();
    smbool CreateGeometry(const GeometryConfig& config, Geometry* geometry);
    void DestroyGeometry(Geometry* geometry);
    Geometry* AcquireFromConfig(const GeometryConfig& geometryConfig);
    GeometryConfig GenerateGeometryConfig(smfloat32 width, smfloat32 height, smuint32 xSegments, smuint32 ySegments, smfloat32 xTile, smfloat32 yTile, smcstring name, smcstring materialName);

private:

    GeometryReference* AcquireFreeGeometrySlot(Geometry*& geometry);
    Geometry* m_DefaultGeometry = nullptr;
    GeometrySystemConfig m_Config;

    std::vector<GeometryReference> m_Geometries ;
};

END_NAMESPACE