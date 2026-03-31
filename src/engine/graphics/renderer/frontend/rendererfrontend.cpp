#include <engine/graphics/renderer/frontend/rendererfrontend.hpp>
#include <engine/graphics/renderer/backend/vulkan/vulkanbackend.hpp>
#include <engine/graphics/systems/shadersystem.hpp>
#include <engine/graphics/systems/materialsystem.hpp>

#include <engine/memory/memory.hpp>
#include <engine/events/eventmanager.hpp>

#include <algorithm>

#if IMGUI_DISPLAY_ENABLED
#include <editor/imgui/imguidrawmodule.hpp>
#endif

#ifdef SM_TOOL
#include <engine/graphics/debug/lightingdebug.hpp>
#endif

BEGIN_NAMESPACE

namespace Graphics
{
    void Renderer::Init()
    {
        if (m_RendererBackend)
        {
            m_NearClip = 0.1f;
            m_FarClip = 1000.0f;
            m_Projection = smMat4::Perspective(Math::Deg2Rad(45.0f), 1280 / 720.0f, 0.1f, 1000.0f);
            m_View = smMat4Translation(smVec3{ 0, 0, -10.0f });
            //TODO : [GRAPHICS] Is this needed?
            //m_View.InverseFastSelf();

            m_RendererBackend->Init();

            smShaderSystem().Create(SM_DEFAULT_SHADER_RESOURCE);
            smShaderSystem().Create("infiniteGridShaderConfig");
            smShaderSystem().Create("pickingShaderConfig");
        }
        else
        {
            hardAssert(false, "Renderer not initialized!");
        }
    }

    void Renderer::SingletonInit()
    {
        Init(RendererType::Vulkan);
    }

    void Renderer::SingletonShutdown()
    {
        Shutdown();
    }

    void Renderer::Init(RendererType rendererType)
    {
        if (rendererType != RendererType::Vulkan && rendererType != RendererType::DirectX12)
        {
            hardAssert(false, "Specified renderer is not supported!");
        }
        switch (rendererType)
        {
            case samaritan::Graphics::RendererType::Vulkan:
            {
                m_RendererBackend = Memory::mnew<VulkanRenderer>(Memory::MemoryTag::MEM_RENDERER);
            }
            break;
            case samaritan::Graphics::RendererType::DirectX12:
            {

            }
            break;
        }
        Init();
    }

    void Renderer::Shutdown()
    {
        if (m_RendererBackend)
        {
            m_RendererBackend->Shutdown();
        }
        
        gpFreeRenderer(m_RendererBackend, sizeof(m_RendererBackend));
    }

    void Renderer::SetGlobalUniforms()
    {
        smShaderSystem().SetUniformByName("projection", &m_Projection);
        smShaderSystem().SetUniformByName("view", &m_View);
#ifdef SM_TOOL
        smVec4 ambientColor = smLightingDebug().GetAmbientColor();
#else
        smVec4 ambientColor = smVec4{ 0.1f, 0.1f, 0.1f, 1.0f };
#endif
        smShaderSystem().SetUniformByName("ambient_color", &ambientColor);
        smVec4 dirLightDirection = smVec4{ -0.577f, -0.577f, -0.577f, 0.0f };
        smVec4 dirLightColor = smVec4{ 1.0f, 1.0f, 1.0f, 1.0f };
        smShaderSystem().SetUniformByName("dir_light_direction", &dirLightDirection);
        smShaderSystem().SetUniformByName("dir_light_color", &dirLightColor);
        smShaderSystem().ApplyGlobalUniforms();
    }

    void Renderer::DrawFrame(RenderData& renderData)
    {
        if (m_RendererBackend == nullptr)
        {
            hardAssert(false, "Renderer not initialized!");
            return;
        }

        if (!m_RendererBackend->BeginFrame(renderData.m_Time))
        {
            return;
        }

        m_DrawList.swap(m_DrawListToDraw);
        m_DrawList.clear();

        // Sort by order then by name. this is neede because infinite grid will render before world objects and objects won't be seen
        std::stable_sort(m_DrawListToDraw.begin(), m_DrawListToDraw.end(),
        [](const DrawCall& a, const DrawCall& b)
        {
            if (a.m_SortOrder != b.m_SortOrder)
            {
                return a.m_SortOrder < b.m_SortOrder;
            }

            return a.m_ShaderName < b.m_ShaderName;
        });

        m_RendererBackend->BeginWorldPass();
#if IMGUI_DISPLAY_ENABLED
        smImguiDrawModule().NewFrame();
#endif

        smstring activeShader;
        for (const DrawCall& call : m_DrawListToDraw)
        {
            if (call.m_ShaderName != activeShader)
            {
                activeShader = call.m_ShaderName;
                smShaderSystem().Use(activeShader);
                SetGlobalUniforms();
            }

            if (call.m_Mesh)
            {
                for (Geometry* geometry : call.m_Mesh->m_Geometries)
                {
                    GeometryData data = {};
                    data.m_Model = call.m_Model;
                    data.m_Geometry = geometry;
                    data.m_ObjectId = call.m_ObjectId;

                    if (geometry->m_Material)
                    {
                        geometry->m_Material->Apply();
                    }

                    smShaderSystem().SetUniformByName("model", &data.m_Model);
                    smShaderSystem().SetUniformByName("overlay_color", &call.m_OverlayColor);
                    m_RendererBackend->DrawGeometry(data);
                }
            }
            else if (call.m_ProceduralVertexCount > 0)
            {
                m_RendererBackend->DrawProcedural(call.m_ProceduralVertexCount);
            }
        }

#if IMGUI_DISPLAY_ENABLED
        smImguiDrawModule().Render();
#endif
        m_RendererBackend->EndWorldPass();

        if (!m_RendererBackend->EndFrame(renderData.m_Time))
        {
            softAssert(false, "Error ending frame!");
        }

        m_RendererBackend->IncrementFrameCount();
    }

    void Renderer::Resize(smuint32 width, smuint32 heigth)
    {
        if (m_RendererBackend != nullptr and heigth != 0 and width != 0)
        {
            m_Projection = smMat4::Perspective(Math::Deg2Rad(45.0f), static_cast<smfloat32>(width) / static_cast<smfloat32>(heigth), m_NearClip, m_FarClip);

            m_RendererBackend->Resize(width, heigth);
        }
    }

    smbool Renderer::CreateTexture(const smuint8* pixels, Texture* texture)
    {
        if (m_RendererBackend != nullptr)
        {
            return m_RendererBackend->CreateTexture(pixels, texture);
        }
        return false;
    }

    void Renderer::DestroyTexture(Texture* texture)
    {
        if (m_RendererBackend != nullptr)
        {
            m_RendererBackend->DestroyTexture(texture);
        }
    }

    smbool Renderer::CreateGeometry(Geometry* geometry, smuint32 vertexCount, const smVert3D* vertices, smuint32 indexCount, const smuint32* indices)
    {
        if (m_RendererBackend != nullptr)
        {
            return m_RendererBackend->CreateGeometry(geometry, vertexCount, vertices, indexCount, indices);
        }
        return false;
    }

    void Renderer::DestroyGeometry(Geometry* geometry)
    {
        if (m_RendererBackend != nullptr)
        {
            m_RendererBackend->DestroyGeometry(geometry);
        }
    }

    void Renderer::SubmitMesh(const Mesh* mesh, const smMat4& model, smuint32 objectId)
    {
        if (mesh != nullptr)
        {
            DrawCall call;
            call.m_Mesh = mesh;
            call.m_Model = model;
            call.m_ObjectId = objectId;
            call.m_ShaderName = SM_DEFAULT_SHADER_NAME;
            m_DrawList.push_back(call);
        }
    }

    void Renderer::SubmitMeshOverlayed(const Mesh* mesh, const smMat4& model, smuint32 objectId, const smVec4& overlay)
    {
        if (mesh != nullptr)
        {
            DrawCall call;
            call.m_Mesh = mesh;
            call.m_Model = model;
            call.m_ObjectId = objectId;
            call.m_OverlayColor = overlay;
            call.m_ShaderName = SM_DEFAULT_SHADER_NAME;
            m_DrawList.push_back(call);
        }
    }

    void Renderer::SubmitProcedural(const smstring& shaderName, smuint32 vertexCount, smuint8 sortOrder)
    {
        DrawCall call;
        call.m_ShaderName = shaderName;
        call.m_ProceduralVertexCount = vertexCount;
        call.m_SortOrder = sortOrder;
        m_DrawList.push_back(call);
    }

    void Renderer::SubmitProceduralOverlayed(const smstring& shaderName, smuint32 vertexCount, const smVec4& overlay)
    {
        DrawCall call;
        call.m_ShaderName = shaderName;
        call.m_ProceduralVertexCount = vertexCount;
        call.m_OverlayColor = overlay;
        m_DrawList.push_back(call);
    }

    void Renderer::DrawProcedural(smuint32 vertexCount)
    {
        if (m_RendererBackend != nullptr)
        {
            m_RendererBackend->DrawProcedural(vertexCount);
        }
    }

    smbool Renderer::CreateShader(Shader* shader)
    {
        if (m_RendererBackend != nullptr)
        {
            return m_RendererBackend->CreateShader(shader);
        }
        return false;
    }

    void Renderer::DestroyShader(Shader* shader)
    {
        if (m_RendererBackend != nullptr)
        {
            m_RendererBackend->DestroyShader(shader);
        }
    }
    smbool Renderer::UseShader(Shader* shader)
    {
        if (m_RendererBackend != nullptr)
        {
            return m_RendererBackend->UseShader(shader);
        }
        return false;
    }

    smuint32 Renderer::PickEntity(smuint32 mouseX, smuint32 mouseY)
    {
        if (m_RendererBackend != nullptr)
        {
            std::vector<GeometryData> geometryData;
            for (const DrawCall& call : m_DrawListToDraw)
            {
                if (!call.m_Mesh)
                {
                    continue;
                }

                for (Geometry* geometry : call.m_Mesh->m_Geometries)
                {
                    GeometryData data = {};
                    data.m_Model = call.m_Model;
                    data.m_Geometry = geometry;
                    data.m_ObjectId = call.m_ObjectId;
                    geometryData.push_back(data);
                }
            }

            return m_RendererBackend->DrawPicking(mouseX, mouseY, m_Projection, m_View, geometryData);
        }
        return 0;
    }
}

END_NAMESPACE