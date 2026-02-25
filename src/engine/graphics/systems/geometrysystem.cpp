#include <engine/graphics/systems/geometrysystem.hpp>

#include <engine/graphics/renderer/frontend/rendererfrontend.hpp>
#include <engine/graphics/systems/materialsystem.hpp>
#include <engine/graphics/systems/materialsystem.hpp>
#include <engine/resources/graphics/geometry.hpp>

#include <iterator>
#include <math/vector.hpp>
#include <utils/logger/log.hpp>

BEGIN_NAMESPACE

smbool GeometrySystem::Init(const GeometrySystemConfig& config)
{
    if (config.m_MaxGeometryCount == 0)
    {
        softAssert(false, "Geometry System Config has max geometry count 0!");
        return false;
    }
    m_Config = config;

    GeometryReference temp;
    std::fill_n(std::back_inserter(m_Geometries), config.m_MaxGeometryCount, temp);

    if (!CreateDefaultGeometry())
    {
        softAssert(false, "Failed to create default geometry!");
        return false;
    }
    return true;
}

void GeometrySystem::Shutdown()
{
}

Geometry* GeometrySystem::AcquireById(smuint32 id)
{
    if (id != SM_INVALID_ID)
    {
        GeometryReference& reference = m_Geometries[id];
        if (reference.m_Geometry.m_Id != SM_INVALID_ID)
        {
            ++reference.m_RefCount;
            return &reference.m_Geometry;
        }
    }

    softAssert(false, "Trying to acquire geometry with invalid id!");
    return m_DefaultGeometry;
}

void GeometrySystem::Release(Geometry* geometry)
{
    smuint32 id = geometry->m_Id;
    if (geometry != nullptr && id != SM_INVALID_ID)
    {
        GeometryReference& reference = m_Geometries[id];
        hardAssert(reference.m_Geometry.m_Id == id, "Geometry id missmatch. This should never happen!");

        if (reference.m_RefCount > 0)
        {
            --reference.m_RefCount;
        }

        if (reference.m_RefCount == 0 && reference.m_ShouldAutoRelease) 
        {
            DestroyGeometry(&reference.m_Geometry);
            reference.m_ShouldAutoRelease = false;
        }
    }
    LogError(LogChannel::Graphics, "Could not release geometry with id %d", id);
}

smbool GeometrySystem::CreateDefaultGeometry()
{
    smVert3D vertices[4];

    constexpr smfloat32 factor = 1.0f;

    vertices[0].m_Position.m_X = -0.5 * factor;
    vertices[0].m_Position.m_Y = -0.5 * factor;
    vertices[0].m_TextureCoordinates.m_X = 0.0f;
    vertices[0].m_TextureCoordinates.m_Y = 0.0f;

    vertices[1].m_Position.m_Y = 0.5 * factor;
    vertices[1].m_Position.m_X = 0.5 * factor;
    vertices[1].m_TextureCoordinates.m_X = 1.0f;
    vertices[1].m_TextureCoordinates.m_Y = 1.0f;

    vertices[2].m_Position.m_X = -0.5 * factor;
    vertices[2].m_Position.m_Y = 0.5 * factor;
    vertices[2].m_TextureCoordinates.m_X = 0.0f;
    vertices[2].m_TextureCoordinates.m_Y = 1.0f;

    vertices[3].m_Position.m_X = 0.5 * factor;
    vertices[3].m_Position.m_Y = -0.5 * factor;
    vertices[3].m_TextureCoordinates.m_X = 1.0f;
    vertices[3].m_TextureCoordinates.m_Y = 0.0f;

    smuint32 indices[6] = { 0, 1, 2, 0, 3, 1 };

    constexpr smuint32 numberOfVertices = 4;
    constexpr smuint32 numberOfIndicies = 6;

    GeometryReference* geometryReference = AcquireFreeGeometrySlot(m_DefaultGeometry);
    if (geometryReference == nullptr)
    {
        softAssert(false, "No free geometry slots available for default geometry!");
        return false;
    }
    geometryReference->m_ShouldAutoRelease = false;

    if (!smRenderer().CreateGeometry(m_DefaultGeometry, numberOfVertices, vertices, numberOfIndicies, indices))
    {
        hardAssert(false, "Cannot create default geometry!");
    }

    // TODO: [GRAPHICS][GEOMETRY] check
    m_DefaultGeometry->m_Material = const_cast<Material*>(&smMaterialSystem().GetDefaultMaterial());

    return true;
}

GeometryReference* GeometrySystem::AcquireFreeGeometrySlot(Geometry*& geometry)
{
    geometry = nullptr;
    smuint32 index = 0;
    for (GeometryReference& reference : m_Geometries)
    {
        if (reference.m_Geometry.m_Id == SM_INVALID_ID)
        {
            reference.m_RefCount = 1;
            geometry = &reference.m_Geometry;
            geometry->m_Id = index;
            return &reference;
        }
        ++index;
    }
    return nullptr;
}

Geometry* GeometrySystem::AcquireFromConfig(const GeometryConfig& geometryConfig)
{
    Geometry* geometry = nullptr;
    GeometryReference* geometryReference = AcquireFreeGeometrySlot(geometry);

    if (geometry == nullptr) 
    {
        softAssert(false, "No free geometry slots available!");
        return nullptr;
    }

    geometryReference->m_ShouldAutoRelease = geometryConfig.m_ShouldAutoRelease;

    if (!CreateGeometry(geometryConfig, geometry))
    {
        softAssert(false, "Failed to create geometry from config!");
        return nullptr;
    }

    return geometry;
}

smbool GeometrySystem::CreateGeometry(const GeometryConfig& config, Geometry* geometry)
{
    if (!smRenderer().CreateGeometry(geometry, config.m_VertexCount, config.m_Vertices, config.m_IndexCount, config.m_Indices)) 
    {
        LogError(LogChannel::Graphics, "Failed to create geometry in renderer!");

        GeometryReference& reference = m_Geometries[geometry->m_Id];
        reference.m_RefCount = 0;
        reference.m_ShouldAutoRelease = false;

        geometry->m_Id = SM_INVALID_ID;
        geometry->m_Generation = SM_INVALID_ID;
        return false;
    }

    std::strncpy(reinterpret_cast<char*>(geometry->m_Name), reinterpret_cast<const char*>(config.m_GeometryName), SM_GEOMETRY_NAME_MAX_LENGTH);

    smcstring materialName = reinterpret_cast<smcstring>(config.m_MaterialName);
    if(std::strlen(materialName) > 0)
    {
        Material* material = smMaterialSystem().Acquire(materialName);
        geometry->m_Material = material;
        if (material == nullptr)
        {
            LogError(LogChannel::Graphics, "Failed to acquire material %s for geometry: %s", materialName, geometry->m_Name);
            geometry->m_Material = const_cast<Material*>(&smMaterialSystem().GetDefaultMaterial());
        }
    }

    return true;
}

void GeometrySystem::DestroyGeometry(Geometry* geometry)
{
    smRenderer().DestroyGeometry(geometry);
    geometry->m_Id = SM_INVALID_ID;
    geometry->m_Generation = SM_INVALID_ID;
    
    if (Material* material = geometry->m_Material)
    {
        if (std::strlen(reinterpret_cast<smcstring>(material->m_Name)) > 0)
        {
            smMaterialSystem().Release(reinterpret_cast<smcstring>(material->m_Name));
        }
        geometry->m_Material = nullptr;
    }
}

GeometryConfig GeometrySystem::GenerateGeometryConfig(smfloat32 width, smfloat32 height, smuint32 xSegments, smuint32 ySegments, smfloat32 xTile, smfloat32 yTile, smcstring name, smcstring materialName)
{
    // Ensure all input parameters are valid and set sensible defaults if not.
    width = (width != 0.0f) ? width : 1.0f;
    height = (height != 0.0f) ? height : 1.0f;
    xSegments = (xSegments > 0) ? xSegments : 1;
    ySegments = (ySegments > 0) ? ySegments : 1;
    xTile = (xTile != 0.0f) ? xTile : 1.0f;
    yTile = (yTile != 0.0f) ? yTile : 1.0f;

    GeometryConfig config;
    constexpr smuint16 vertsPerSegment = 4;
    constexpr smuint16 indiciesPerSegment = 6; //bad
    const smuint32 segmentArea = xSegments * ySegments;
    config.m_VertexCount = segmentArea * vertsPerSegment;
    config.m_Vertices = static_cast<smVert3D*>(Memory::GPAllocate(sizeof(smVert3D) * config.m_VertexCount, Memory::MemoryTag::MEM_ARRAY));
    smZero(config.m_Vertices, config.m_VertexCount * sizeof(smVert3D));
    config.m_IndexCount = segmentArea * indiciesPerSegment;
    config.m_Indices = static_cast<smuint32*>(Memory::GPAllocate(sizeof(smuint32) * config.m_IndexCount, Memory::MemoryTag::MEM_ARRAY));
    smZero(config.m_Indices, config.m_IndexCount * sizeof(smuint32));

    //TODO : [GRAPHICS][GEOMETRY] double vertex allocation
    smfloat32 segmentWidth = width / xSegments;
    smfloat32 segmentHeight = height / ySegments;
    smfloat32 halfWidth = width * 0.5f;
    smfloat32 halfHeight = height * 0.5f;

    for (smuint32 y = 0; y < ySegments; ++y) 
    {
        for (smuint32 x = 0; x < xSegments; ++x)
        {
            smfloat32 xMin = (x * segmentWidth) - halfWidth;
            smfloat32 yMin = (y * segmentHeight) - halfHeight;
            smfloat32 xMax = xMin + segmentWidth;
            smfloat32 yMax = yMin + segmentHeight;
            smfloat32 uvxMin = (x / (smfloat32)xSegments) * xTile;
            smfloat32 uvyMin = (y / (smfloat32)ySegments) * yTile;
            smfloat32 uvxMax = ((x + 1) / (smfloat32)xSegments) * xTile;
            smfloat32 uvyMax = ((y + 1) / (smfloat32)ySegments) * yTile;

            smuint32 vertexOffset = ((y * xSegments) + x) * vertsPerSegment;

            smVert3D& vec0 = config.m_Vertices[vertexOffset + 0];
            smVert3D& vec1 = config.m_Vertices[vertexOffset + 1];
            smVert3D& vec2 = config.m_Vertices[vertexOffset + 2];
            smVert3D& vec3 = config.m_Vertices[vertexOffset + 3];

            vec0.m_Position.m_X = xMin;
            vec0.m_Position.m_Y = yMin;
            vec0.m_Position.m_Z = 0.0;
            vec0.m_TextureCoordinates.m_X = uvxMin;
            vec0.m_TextureCoordinates.m_Y = uvyMin;

            vec1.m_Position.m_X = xMax;
            vec1.m_Position.m_Y = yMax;
            vec1.m_Position.m_Z = 0.0;
            vec1.m_TextureCoordinates.m_X = uvxMax;
            vec1.m_TextureCoordinates.m_Y = uvyMax;

            vec2.m_Position.m_X = xMin;
            vec2.m_Position.m_Y = yMax;
            vec2.m_Position.m_Z = 0.0;
            vec2.m_TextureCoordinates.m_X = uvxMin;
            vec2.m_TextureCoordinates.m_Y = uvyMax;

            vec3.m_Position.m_X = xMax;
            vec3.m_Position.m_Y = yMin;
            vec3.m_Position.m_Z = 0.0;
            vec3.m_TextureCoordinates.m_X = uvxMax;
            vec3.m_TextureCoordinates.m_Y = uvyMin;

            // Generate indices
            const smuint32 indexOffset = ((y * xSegments) + x) * indiciesPerSegment;
            config.m_Indices[indexOffset + 0] = vertexOffset + 0;
            config.m_Indices[indexOffset + 1] = vertexOffset + 1;
            config.m_Indices[indexOffset + 2] = vertexOffset + 2;
            config.m_Indices[indexOffset + 3] = vertexOffset + 0;
            config.m_Indices[indexOffset + 4] = vertexOffset + 3;
            config.m_Indices[indexOffset + 5] = vertexOffset + 1;
        }
    }

    if (name != nullptr && std::strlen(name) > 0) 
    {
        std::strncpy(reinterpret_cast<char*>(config.m_GeometryName), name, SM_GEOMETRY_NAME_MAX_LENGTH);
    }
    else 
    {
        std::strncpy(reinterpret_cast<char*>(config.m_GeometryName), SM_DEFAULT_GEOMETRY_NAME, SM_GEOMETRY_NAME_MAX_LENGTH);
    }

    if (materialName != nullptr && std::strlen(materialName) > 0)
    {
        std::strncpy(reinterpret_cast<char*>(config.m_MaterialName), materialName, SM_MATERIAL_NAME_MAX_LENGTH);
    }
    else {
        std::strncpy(reinterpret_cast<char*>(config.m_MaterialName), SM_DEFAULT_MATERIAL_NAME, SM_MATERIAL_NAME_MAX_LENGTH);
    }

    return config;
}

END_NAMESPACE