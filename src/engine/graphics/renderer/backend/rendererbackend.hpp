#pragma once
#include <defines.hpp>
#include <engine/graphics/renderer/renderertype.inl>
#include <engine/resources/graphics/texture.hpp>
#include <engine/resources/graphics/shader.hpp>

#include <math/matrix.hpp>
#include <math/vector.hpp>
#include <objecttemplates/singleton.hpp>
#include <objecttemplates/shutdownable.hpp>



BEGIN_NAMESPACE
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
		virtual void UpdateGlobalState(smMat4 projection, smMat4 view, smVec3 viewPosition, smVec4 ambientColor, smint32 mode) = 0;
		virtual void DrawGeometry(const GeometryData& data) = 0;
		virtual smbool CreateTexture(const smuint8* pixels, Texture* texture) = 0;
		virtual void DestroyTexture(Texture* texture) = 0;
		virtual smbool CreateMaterial(Material* material) = 0;
		virtual void DestroyMaterial(Material* material) = 0;
		virtual smbool CreateGeometry(Geometry* geometry, smuint32 vertexCount, const smVert3D* vertices, smuint32 indexCount, const smuint32* indices) = 0;
		virtual void DestroyGeometry(Geometry* geometry) = 0;
		virtual smbool CreateObjectShader(Shader* shader) = 0;
		virtual void DestroyObjectShader(Shader* shader) = 0;

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