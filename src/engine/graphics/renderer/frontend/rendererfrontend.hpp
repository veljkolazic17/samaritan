#pragma once
#include <defines.hpp>

#include <engine/graphics/renderer/backend/rendererbackend.hpp>
#include <engine/graphics/renderer/renderertype.inl>

#include <engine/resources/graphics/shader.hpp>
#include <engine/resources/graphics/mesh.hpp>
#include <math/matrix.hpp>
#include <objecttemplates/shutdownable.hpp>
#include <objecttemplates/singleton.hpp>

#define smRenderer()	::samaritan::Graphics::Renderer::GetInstance()

BEGIN_NAMESPACE

namespace Graphics
{
    class Renderer SINGLETON(Renderer), public Shutdownable
    {
    public:
        struct DrawCall //TODO : [FUCKED] this overlay shit should be moved somewhere else
        {
            const Mesh* m_Mesh = nullptr;
            smuint32 m_ProceduralVertexCount = 0;
            smMat4 m_Model = smMat4_identity;
            smuint32 m_ObjectId = 0;
            smVec4 m_OverlayColor = { 0.0f, 0.0f, 0.0f, 0.0f };
            smstring m_ShaderName;
            smuint8 m_SortOrder = 0; // lower draws first
        };

        SINGLETON_CONSTRUCTOR(Renderer);

        void Init() override;
        void Shutdown() override;
        void SingletonInit() override;
        void SingletonShutdown() override;

        void Init(RendererType rendererType);

        void Resize(smuint32 width, smuint32 heigth);
        void DrawFrame(RenderData& renderData);

        void SubmitMesh(const Mesh* mesh, const smMat4& model, smuint32 objectId);
        void SubmitMeshOverlayed(const Mesh* mesh, const smMat4& model, smuint32 objectId, const smVec4& overlay);
        void SubmitProcedural(const smstring& shaderName, smuint32 vertexCount, smuint8 sortOrder = 0);
        void SubmitProceduralOverlayed(const smstring& shaderName, smuint32 vertexCount, const smVec4& overlay);

        SM_INLINE smMat4& GetProjection() { return m_Projection; }
        SM_INLINE smMat4& GetView() { return m_View; }

        SM_INLINE void SetProjection(const smMat4& projection) { m_Projection = projection; }
        SM_INLINE void SetView(const smMat4& view) { m_View = view; }

        smbool CreateTexture(const smuint8* pixels, Texture* texture);
        void DestroyTexture(Texture* texture);

        smbool CreateGeometry(Geometry* geometry, smuint32 vertexCount, const smVert3D* vertices, smuint32 indexCount, const smuint32* indices);
        void DestroyGeometry(Geometry* geometry);
        void DrawProcedural(smuint32 vertexCount);

        smbool CreateShader(Shader* shader);
        void DestroyShader(Shader* shader);

        smbool UseShader(Shader* shader);

        smuint32 PickEntity(smuint32 mouseX, smuint32 mouseY);

        SM_INLINE const std::vector<DrawCall>& GetDrawList() { return m_DrawList; }

        HACK(SM_INLINE RendererBackend* GetRendererBackend() { return m_RendererBackend; })

    private:
        void SetGlobalUniforms();

        smMat4 m_Projection;
        smMat4 m_View;
        RendererBackend* m_RendererBackend = nullptr;
        smfloat32 m_NearClip;
        smfloat32 m_FarClip;
        std::vector<DrawCall> m_DrawList;
        std::vector<DrawCall> m_DrawListToDraw;
    };
}

END_NAMESPACE