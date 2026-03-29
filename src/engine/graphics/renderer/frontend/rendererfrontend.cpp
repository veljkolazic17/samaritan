#include <engine/graphics/renderer/frontend/rendererfrontend.hpp>
#include <engine/graphics/renderer/backend/vulkan/vulkanbackend.hpp>
#include <engine/graphics/systems/shadersystem.hpp>
#include <engine/graphics/systems/materialsystem.hpp>

#include <engine/memory/memory.hpp>
#include <engine/events/eventmanager.hpp>

#if IMGUI_DISPLAY_ENABLED
#include <editor/imgui/imguidrawmodule.hpp>
#endif

#ifdef SM_TOOL
#include <engine/graphics/debug/lightingdebug.hpp>
#endif

#ifdef TEST_CODE_ENABLED
#include <math/matrix.hpp>
#include <math/math.hpp>
#include <math/vector.hpp>
#include <engine/graphics/systems/geometrysystem.hpp>
#ifdef SM_TOOL
#include <engine/graphics/debug/transformdebug.hpp>
#endif
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

    void Renderer::DrawFrame(RenderData& renderData)
    {
        if (m_RendererBackend != nullptr)
        {
            if (m_RendererBackend->BeginFrame(renderData.m_Time))
            {
                m_DrawList.swap(m_DrawListToDraw);
                m_DrawList.clear();

                m_RendererBackend->BeginWorldPass();
#if IMGUI_DISPLAY_ENABLED
                smImguiDrawModule().NewFrame();
#endif
                // --- World pass ---

                smShaderSystem().Use(SM_DEFAULT_SHADER_NAME);
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

                constexpr smfloat32 meshScale = 1.0f;
#ifdef SM_TOOL
                smMat4 model = smTransformDebug().GetModelMatrix();
#else
                smMat4 model = smMat4_identity;
#endif
                model[0] = model[0] * meshScale;
                model[1] = model[1] * meshScale;
                model[2] = model[2] * meshScale;
                if (!m_DrawListToDraw.empty())
                {
                    for (const DrawCall& call : m_DrawListToDraw)
                    {
                        for (Geometry* geometry : call.m_Mesh->m_Geometries)
                        {
                            GeometryData data = {};
                            data.m_Model = call.m_Model;
                            data.m_Geometry = geometry;
                            data.m_ObjectId = geometry->m_Id;
                            if (geometry->m_Material)
                            {
                                geometry->m_Material->Apply();
                            }
                            smShaderSystem().SetUniformByName("model", &data.m_Model);
                            m_RendererBackend->DrawGeometry(data);
                        }
                    }
                }

                smShaderSystem().Use("InfiniteGridShader");
                smShaderSystem().SetUniformByName("projection", &m_Projection);
                smShaderSystem().SetUniformByName("view", &m_View);
                smShaderSystem().ApplyGlobalUniforms();
                smRenderer().DrawProcedural(6);

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
        }
        else
        {
            hardAssert(false, "Renderer not initialized!");
        }
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
            m_DrawList.push_back({ mesh, model, objectId});
        }
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

    //TODO : [FUCKED]This is more than pure shit. Is there way not to iterate over all geometries again????
    smuint32 Renderer::PickEntity(smuint32 mouseX, smuint32 mouseY)
    {
        if (m_RendererBackend != nullptr)
        {
            std::vector<GeometryData> geometryData;
            for (const DrawCall& call : m_DrawListToDraw)
            {
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