#pragma once
#include <defines.hpp>

#include <engine/graphics/renderer/backend/rendererbackend.hpp>
#include <engine/graphics/renderer/renderertype.inl>

#include <engine/resources/graphics/shader.hpp>
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
        SINGLETON_CONSTRUCTOR(Renderer);

        void Init() override;
        void Shutdown() override;

        void Init(RendererType rendererType);

        void Resize(smuint32 width, smuint32 heigth);
        void DrawFrame(RenderData& renderData);

        SM_INLINE smMat4& GetProjection() { return m_Projection; }
        SM_INLINE smMat4& GetView() { return m_View; }

        SM_INLINE void SetProjection(const smMat4& projection) { m_Projection = projection; }
        SM_INLINE void SetView(const smMat4& view) { m_View = view; }

        smbool CreateTexture(const smuint8* pixels, Texture* texture);
        void DestroyTexture(Texture* texture);

        smbool CreateMaterial(Material* material);
        void DestroyMaterial(Material* material);

        smbool CreateGeometry(Geometry* geometry, smuint32 vertexCount, const smVert3D* vertices, smuint32 indexCount, const smuint32* indices);
        void DestroyGeometry(Geometry* geometry);

        smbool CreateObjectShader(Shader* shader);
        void DestroyObjectShader(Shader* shader);

        smbool UseObjectShader(Shader* shader);

#ifdef TEST_CODE_ENABLED
        SM_INLINE void SetGeometry(Geometry* geometry) { m_Geometry = geometry; }
        SM_INLINE Geometry* GetGeometry() { return m_Geometry; }
#endif
    private:
        smMat4 m_Projection;
        smMat4 m_View;
        RendererBackend* m_RendererBackend = nullptr;
        smfloat32 m_NearClip;
        smfloat32 m_FarClip;
#ifdef TEST_CODE_ENABLED
        Geometry* m_Geometry = nullptr;
#endif
    };
}

END_NAMESPACE