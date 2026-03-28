#include <engine/resources/graphics/mesh.hpp>

#include <engine/graphics/systems/geometrysystem.hpp>
#include <engine/graphics/systems/materialsystem.hpp>
#include <engine/graphics/systems/texturesystem.hpp>
#include <engine/resources/graphics/shader.hpp>
#include <engine/resources/graphics/material.hpp>

#include <utils/logger/log.hpp>
#include <math/math.hpp>

#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_IMPLEMENTATION
#include <tiny_gltf.h>

#include <format>
#include <fstream>
#include <functional>
#include <vector>

BEGIN_NAMESPACE

static bool IsImageExtension(const std::string& path)
{
    auto dot = path.rfind('.');
    if (dot == std::string::npos) return false;
    std::string ext = path.substr(dot + 1);
    for (auto& c : ext) c = static_cast<char>(::tolower(static_cast<unsigned char>(c)));
    return ext == "jpg" || ext == "jpeg" || ext == "png" || ext == "bmp"
        || ext == "gif" || ext == "tga" || ext == "hdr" || ext == "exr";
}

static const float* GetAccessorPtr(const tinygltf::Model& model, const tinygltf::Accessor& accessor, size_t index)
{
    const tinygltf::BufferView& view = model.bufferViews[accessor.bufferView];
    const tinygltf::Buffer& buffer   = model.buffers[view.buffer];
    size_t stride                    = accessor.ByteStride(view);
    return reinterpret_cast<const float*>(&buffer.data[view.byteOffset + accessor.byteOffset + index * stride]);
}

// Column-major 4x4 multiply: out = a * b
static void MatMul4(const float a[16], const float b[16], float out[16])
{
    for (int col = 0; col < 4; ++col)
        for (int row = 0; row < 4; ++row)
        {
            float sum = 0;
            for (int k = 0; k < 4; ++k)
                sum += a[k * 4 + row] * b[col * 4 + k];
            out[col * 4 + row] = sum;
        }
}

// Build column-major TRS matrix from a glTF node (glTF spec §5.21)
static void NodeLocalTransform(const tinygltf::Node& node, float m[16])
{
    if (!node.matrix.empty())
    {
        for (int i = 0; i < 16; ++i) m[i] = static_cast<float>(node.matrix[i]);
        return;
    }

    float tx = 0, ty = 0, tz = 0;
    float qx = 0, qy = 0, qz = 0, qw = 1;
    float sx = 1, sy = 1, sz = 1;

    if (node.translation.size() == 3) { tx = (float)node.translation[0]; ty = (float)node.translation[1]; tz = (float)node.translation[2]; }
    if (node.rotation.size()    == 4) { qx = (float)node.rotation[0];    qy = (float)node.rotation[1];    qz = (float)node.rotation[2];    qw = (float)node.rotation[3]; }
    if (node.scale.size()       == 3) { sx = (float)node.scale[0];       sy = (float)node.scale[1];       sz = (float)node.scale[2]; }

    const float xx = 2*qx*qx, yy = 2*qy*qy, zz = 2*qz*qz;
    const float xy = 2*qx*qy, xz = 2*qx*qz, yz = 2*qy*qz;
    const float wx = 2*qw*qx, wy = 2*qw*qy, wz = 2*qw*qz;

    m[0]  = (1-yy-zz)*sx; m[1]  = (xy+wz)*sx;  m[2]  = (xz-wy)*sx;  m[3]  = 0;
    m[4]  = (xy-wz)*sy;   m[5]  = (1-xx-zz)*sy; m[6]  = (yz+wx)*sy;  m[7]  = 0;
    m[8]  = (xz+wy)*sz;   m[9]  = (yz-wx)*sz;   m[10] = (1-xx-yy)*sz;m[11] = 0;
    m[12] = tx;           m[13] = ty;            m[14] = tz;           m[15] = 1;
}

static smVec3 TransformPoint(const float m[16], smVec3 p)
{
    return smVec3{ m[0]*p.m_X + m[4]*p.m_Y + m[8]*p.m_Z  + m[12],
                   m[1]*p.m_X + m[5]*p.m_Y + m[9]*p.m_Z  + m[13],
                   m[2]*p.m_X + m[6]*p.m_Y + m[10]*p.m_Z + m[14] };
}

static smVec3 TransformDirection(const float m[16], smVec3 d)
{
    return smVec3{ m[0]*d.m_X + m[4]*d.m_Y + m[8]*d.m_Z,
                   m[1]*d.m_X + m[5]*d.m_Y + m[9]*d.m_Z,
                   m[2]*d.m_X + m[6]*d.m_Y + m[10]*d.m_Z };
}

void Mesh::OnLoad()
{
    std::string path = std::format("assets/meshes/{}/{}.gltf", m_Name, m_Name);

    tinygltf::Model model;
    tinygltf::TinyGLTF loader;
    std::string err, warn;

    // Skip image loading — textures are managed by our own texture system.
    // Override filesystem callbacks so image file reads silently succeed with empty data,
    // preventing "File not found" warnings. Non-image files (gltf, bin) are read normally.
    tinygltf::FsCallbacks fsCallbacks{};
    fsCallbacks.user_data = nullptr;
    fsCallbacks.FileExists = [](const std::string& path, void*) -> bool
    {
        if (IsImageExtension(path)) return true;
        return static_cast<bool>(std::ifstream(path));
    };

    fsCallbacks.ExpandFilePath = [](const std::string& path, void*) -> std::string { return path; };
    fsCallbacks.ReadWholeFile = [](std::vector<unsigned char>* out, std::string* err, const std::string& path, void*) -> bool
    {
        if (IsImageExtension(path))
        {
            out->clear();
            return true;
        }

        std::ifstream f(path, std::ios::binary | std::ios::ate);
        if (!f)
        {
            if (err)
            {
                *err = "Cannot open: " + path; return false;
            }
        }

        out->resize(static_cast<size_t>(f.tellg()));
        f.seekg(0);
        return f.read(reinterpret_cast<char*>(out->data()), static_cast<std::streamsize>(out->size())).good();
    };

    fsCallbacks.WriteWholeFile = nullptr;
    loader.SetFsCallbacks(fsCallbacks);

    // Install a no-op image decoder so tinygltf never tries to decode image bytes.
    // We load textures ourselves via stbi in our own texture system.
    loader.SetImageLoader(
        [](tinygltf::Image*, const int, std::string*, std::string*,
           int, int, const unsigned char*, int, void*) -> bool { return true; },
        nullptr);

    const bool isBinary = m_Name.size() >= 4 && m_Name.substr(m_Name.size() - 4) == ".glb";
    const bool loaded = isBinary
        ? loader.LoadBinaryFromFile(&model, &err, &warn, path)
        : loader.LoadASCIIFromFile(&model, &err, &warn, path);

    if (!loaded)
    {
        LogError(LogChannel::Resource, "Failed to load glTF '%s': %s", path.c_str(), err.c_str());
        m_State = ResourceState::Error;
        return;
    }

    if (!warn.empty())
        LogWarning(LogChannel::Resource, "glTF '%s': %s", path.c_str(), warn.c_str());

    // Directory containing the glTF file — used to resolve relative image URIs
    const std::string meshDir = std::format("assets/meshes/{}/", m_Name);

    // ProcessPrimitive: build geometry from one glTF primitive, baking worldTransform into vertices
    auto ProcessPrimitive = [&](const tinygltf::Primitive& primitive, const float worldTransform[16])
    {
        if (primitive.indices < 0)
        {
            LogWarning(LogChannel::Resource, "Skipping non-indexed primitive in '%s'", m_Name.c_str());
            return;
        }

        auto posIt = primitive.attributes.find("POSITION");
        if (posIt == primitive.attributes.end())
        {
            LogWarning(LogChannel::Resource, "Primitive has no POSITION, skipping");
            return;
        }

        const tinygltf::Accessor& posAccessor = model.accessors[posIt->second];
        const size_t vertexCount = posAccessor.count;

        auto normalIt  = primitive.attributes.find("NORMAL");
        auto texIt     = primitive.attributes.find("TEXCOORD_0");
        auto tangentIt = primitive.attributes.find("TANGENT");

        const tinygltf::Accessor* normalAccessor  = normalIt  != primitive.attributes.end() ? &model.accessors[normalIt->second]  : nullptr;
        const tinygltf::Accessor* texAccessor     = texIt     != primitive.attributes.end() ? &model.accessors[texIt->second]     : nullptr;
        const tinygltf::Accessor* tangentAccessor = tangentIt != primitive.attributes.end() ? &model.accessors[tangentIt->second] : nullptr;

        std::vector<smVert3D> vertices(vertexCount);

        for (size_t i = 0; i < vertexCount; ++i)
        {
            const float* pos = GetAccessorPtr(model, posAccessor, i);
            vertices[i].m_Position = TransformPoint(worldTransform, smVec3{ pos[0], pos[1], pos[2] });

            if (normalAccessor)
            {
                const float* n = GetAccessorPtr(model, *normalAccessor, i);
                vertices[i].m_Normal = TransformDirection(worldTransform, smVec3{ n[0], n[1], n[2] });
            }

            if (texAccessor)
            {
                const float* uv = GetAccessorPtr(model, *texAccessor, i);
                vertices[i].m_TextureCoordinates = smVec2{ uv[0], 1.0f - uv[1] };
            }

            if (tangentAccessor)
            {
                const float* t = GetAccessorPtr(model, *tangentAccessor, i);
                vertices[i].m_Tangent   = TransformDirection(worldTransform, smVec3{ t[0], t[1], t[2] });
                vertices[i].m_Bitangent = smVec3::Cross(vertices[i].m_Normal, vertices[i].m_Tangent) * t[3];
            }
        }

        // Read index buffer
        const tinygltf::Accessor&   indexAccessor = model.accessors[primitive.indices];
        const tinygltf::BufferView& indexView     = model.bufferViews[indexAccessor.bufferView];
        const unsigned char*        indexData     = &model.buffers[indexView.buffer].data[indexView.byteOffset + indexAccessor.byteOffset];

        std::vector<smuint32> indices(indexAccessor.count);

        if (indexAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
        {
            const uint16_t* src = reinterpret_cast<const uint16_t*>(indexData);
            for (size_t i = 0; i < indexAccessor.count; ++i) indices[i] = src[i];
        }
        else if (indexAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)
        {
            const uint32_t* src = reinterpret_cast<const uint32_t*>(indexData);
            for (size_t i = 0; i < indexAccessor.count; ++i) indices[i] = src[i];
        }
        else if (indexAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE)
        {
            for (size_t i = 0; i < indexAccessor.count; ++i) indices[i] = indexData[i];
        }

        if (!tangentAccessor)
        {
            for (size_t i = 0; i + 2 < indices.size(); i += 3)
            {
                smVert3D& v0 = vertices[indices[i]];
                smVert3D& v1 = vertices[indices[i + 1]];
                smVert3D& v2 = vertices[indices[i + 2]];
                smVec3 tangent, bitangent;
                Math::ComputeTangentBitangent(v0.m_Position, v1.m_Position, v2.m_Position, v0.m_TextureCoordinates, v1.m_TextureCoordinates, v2.m_TextureCoordinates, tangent, bitangent);
                v0.m_Tangent = v1.m_Tangent = v2.m_Tangent = tangent;
                v0.m_Bitangent = v1.m_Bitangent = v2.m_Bitangent = bitangent;
            }
        }

        // Build Material directly from glTF data
        Material* primitiveMaterial = nullptr;
        if (primitive.material >= 0)
        {
            const tinygltf::Material& gltfMat = model.materials[primitive.material];

            Material mat{};
            mat.m_ShaderName = SM_DEFAULT_SHADER_NAME;
            mat.m_Shininess  = 32.0f;

            const std::string matName = std::format("{}__mat_{}", m_Name, primitive.material);
            mat.m_Name = matName;

            // Diffuse colour factor
            mat.m_DiffuseColor = smVec4_one;
            {
                const auto& bcf = gltfMat.pbrMetallicRoughness.baseColorFactor;
                if (bcf.size() == 4)
                    mat.m_DiffuseColor = smVec4{ (float)bcf[0], (float)bcf[1], (float)bcf[2], (float)bcf[3] };

                auto extIt = gltfMat.extensions.find("KHR_materials_pbrSpecularGlossiness");
                if (extIt != gltfMat.extensions.end() && extIt->second.Has("diffuseFactor"))
                {
                    const tinygltf::Value& df = extIt->second.Get("diffuseFactor");
                    if (df.IsArray() && df.ArrayLen() == 4)
                    {
                        mat.m_DiffuseColor = smVec4{ (float)df.Get(0).GetNumberAsDouble(), (float)df.Get(1).GetNumberAsDouble(), (float)df.Get(2).GetNumberAsDouble(), (float)df.Get(3).GetNumberAsDouble() };
                    }
                }
            }

            // Diffuse texture
            mat.m_DiffuseMap.m_Type = TextureUsageType::TEXTURE_USAGE_MAP_DIFFUSE;
            {
                int diffuseTexIdx = gltfMat.pbrMetallicRoughness.baseColorTexture.index;
                if (diffuseTexIdx < 0)
                {
                    auto extIt = gltfMat.extensions.find("KHR_materials_pbrSpecularGlossiness");
                    if (extIt != gltfMat.extensions.end() && extIt->second.Has("diffuseTexture"))
                    {
                        const tinygltf::Value& dt = extIt->second.Get("diffuseTexture");
                        if (dt.Has("index")) diffuseTexIdx = dt.Get("index").Get<int>();
                    }
                }
                if (diffuseTexIdx >= 0)
                {
                    int imgIdx = model.textures[diffuseTexIdx].source;
                    if (imgIdx >= 0 && !model.images[imgIdx].uri.empty())
                        mat.m_DiffuseMap.m_Texture = smTextureSystem().Acquire(meshDir + model.images[imgIdx].uri, true);
                }
                if (!mat.m_DiffuseMap.m_Texture.IsValid())
                    mat.m_DiffuseMap.m_Texture = smTextureSystem().GetWhiteTexture();
            }

            // Normal texture
            mat.m_NormalMap.m_Type = TextureUsageType::TEXTURE_USAGE_MAP_NORMAL;
            if (gltfMat.normalTexture.index >= 0)
            {
                int imgIdx = model.textures[gltfMat.normalTexture.index].source;
                if (imgIdx >= 0 && !model.images[imgIdx].uri.empty())
                    mat.m_NormalMap.m_Texture = smTextureSystem().Acquire(meshDir + model.images[imgIdx].uri, true);
            }
            if (!mat.m_NormalMap.m_Texture.IsValid())
                mat.m_NormalMap.m_Texture = smTextureSystem().GetWhiteTexture();

            // Specular texture — KHR_materials_pbrSpecularGlossiness.specularGlossinessTexture
            mat.m_SpecularMap.m_Type = TextureUsageType::TEXTURE_USAGE_MAP_SPECULAR;
            {
                auto extIt = gltfMat.extensions.find("KHR_materials_pbrSpecularGlossiness");
                if (extIt != gltfMat.extensions.end() && extIt->second.Has("specularGlossinessTexture"))
                {
                    const tinygltf::Value& sgt = extIt->second.Get("specularGlossinessTexture");
                    if (sgt.Has("index"))
                    {
                        int imgIdx = model.textures[sgt.Get("index").Get<int>()].source;
                        if (imgIdx >= 0 && !model.images[imgIdx].uri.empty())
                            mat.m_SpecularMap.m_Texture = smTextureSystem().Acquire(meshDir + model.images[imgIdx].uri, true);
                    }
                }
            }
            if (!mat.m_SpecularMap.m_Texture.IsValid())
            {
                mat.m_SpecularMap.m_Texture = smTextureSystem().GetWhiteTexture();
            }

            primitiveMaterial = smMaterialSystem().Register(std::move(mat));
        }

        GeometryConfig config{};
        config.m_VertexCount = static_cast<smuint32>(vertices.size());
        config.m_IndexCount  = static_cast<smuint32>(indices.size());
        config.m_Vertices    = vertices.data();
        config.m_Indices     = indices.data();
        config.m_ShouldAutoRelease = false;
        std::snprintf(reinterpret_cast<char*>(config.m_GeometryName), SM_GEOMETRY_NAME_MAX_LENGTH, "%s_%zu", m_Name.c_str(), m_Geometries.size());
        config.m_MaterialName[0] = 0;

        Geometry* geometry = smGeometrySystem().AcquireFromConfig(config);
        if (!geometry)
        {
            LogError(LogChannel::Resource, "Failed to upload geometry for primitive in '%s'", m_Name.c_str());
            return;
        }

        geometry->m_Material = primitiveMaterial
            ? primitiveMaterial
            : const_cast<Material*>(&smMaterialSystem().GetDefaultMaterial());

        m_Geometries.push_back(geometry);
    };

    // Traverse the scene node tree, accumulating transforms, and process each mesh's primitives
    std::function<void(int, const float*)> ProcessNode = [&](int nodeIdx, const float parentTransform[16])
    {
        const tinygltf::Node& node = model.nodes[nodeIdx];

        float localTransform[16];
        NodeLocalTransform(node, localTransform);

        float worldTransform[16];
        MatMul4(parentTransform, localTransform, worldTransform);

        if (node.mesh >= 0)
        {
            const tinygltf::Mesh& gltfMesh = model.meshes[node.mesh];
            for (const tinygltf::Primitive& primitive : gltfMesh.primitives)
            {
                ProcessPrimitive(primitive, worldTransform);
            }
        }

        for (int childIdx : node.children)
        {
            ProcessNode(childIdx, worldTransform);
        }
    };

    float identity[16] = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };

    if (!model.scenes.empty())
    {
        const tinygltf::Scene& scene = model.scenes[model.defaultScene >= 0 ? model.defaultScene : 0];
        for (int rootNode : scene.nodes)
        {
            ProcessNode(rootNode, identity);
        }
    }
    else
    {
        // No scene — fall back to processing all root nodes (nodes not referenced as children)
        for (int i = 0; i < (int)model.nodes.size(); ++i)
        {
            ProcessNode(i, identity);
        }
    }

    if (m_Geometries.empty())
    {
        LogError(LogChannel::Resource, "Mesh '%s' has no valid geometries", m_Name.c_str());
        m_State = ResourceState::Error;
        return;
    }

    m_State = ResourceState::Loaded;
}

void Mesh::OnUnload()
{
    for (Geometry* geometry : m_Geometries)
    {
        smGeometrySystem().Release(geometry);
    }
    m_Geometries.clear();
}

END_NAMESPACE
