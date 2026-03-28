#pragma once
#include <defines.hpp>
#include <engine/graphics/renderer/renderertype.inl>
#include <engine/resources/graphics/texture.hpp>
#include <engine/resources/graphics/shader.hpp>

#include <math/matrix.hpp>
#include <math/vector.hpp>
#include <objecttemplates/shutdownable.hpp>

BEGIN_NAMESPACE

class Geometry;

namespace Graphics
{
    //TODO : [GRAPHICS][RENDERER] Make this compile time polymorphic
    class RendererBackend : public Shutdownable
    {
    public:
        virtual RendererType GetRendererType() = 0;

        SM_INLINE smuint64 GetFrameCount() { return m_FrameCount; }
        SM_INLINE smuint64 SetFrameCount(smuint64 frameCount) { m_FrameCount = frameCount; }
        SM_INLINE smuint64 IncrementFrameCount() { return ++m_FrameCount; }

        virtual void Resize(smuint32 width, smuint32 heigth) = 0;
        virtual smbool BeginFrame(Time time) = 0;
        virtual smbool EndFrame(Time time) = 0;
        virtual void BeginWorldPass() = 0;
        virtual void EndWorldPass() = 0;
        virtual void DrawGeometry(const GeometryData& data) = 0;
        virtual void DrawProcedural(smuint32 vertexCount) = 0;
        virtual smbool CreateTexture(const smuint8* pixels, Texture* texture) = 0;
        virtual void DestroyTexture(Texture* texture) = 0;
        virtual smbool CreateGeometry(Geometry* geometry, smuint32 vertexCount, const smVert3D* vertices, smuint32 indexCount, const smuint32* indices) = 0;
        virtual void DestroyGeometry(Geometry* geometry) = 0;

        virtual smbool CreateShader(Shader* shader) = 0;
        virtual void DestroyShader(Shader* shader) = 0;
        virtual smbool UseShader(Shader* shader) = 0;
        virtual smbool ShaderBindGlobals(Shader* shader) = 0;
        virtual smbool ShaderSetUniform(Shader* shader, const ShaderUniform& uniform, const void* value) = 0;
        virtual smbool ShaderApplyGlobals(Shader* shader) = 0;
        virtual smbool ShaderApplyInstances(Shader* shader) = 0;
        virtual smbool ShaderBindInstance(Shader* shader, smuint32 instanceId) = 0;
        virtual smbool InitShader(Shader* shader) = 0;
        virtual smbool ShaderAcquireInstanceResources(Shader* shader, smuint32& instanceId) = 0;
        virtual smbool ShaderReleaseInstanceResources(Shader* shader, smuint32 instanceId) = 0;

        virtual smuint32 DrawPicking(smuint32 mouseX, smuint32 mouseY, const smMat4& projection, const smMat4& view, const std::vector<GeometryData>& drawCalls) { return 0; }

    protected:
        RendererType m_RendererType;
        smbool m_Initialized = false;
        smuint64 m_FrameCount = 0;
    };
}

END_NAMESPACE

#define DEFINE_RENDERER(rendererType)				\
    RendererType GetRendererType() override			\
    {                                           	\
        return rendererType;            			\
    }