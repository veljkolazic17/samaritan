#pragma once
#include <defines.hpp>

#include <engine/graphics/renderer/renderertype.inl>
#include <engine/graphics/renderer/backend/rendererbackend.hpp>

#include <objecttemplates/singleton.hpp>
#include <objecttemplates/shutdownable.hpp>
#include <math/matrix.hpp>

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

		void Resize(muint32 width, muint32 heigth);
		void DrawFrame(RenderData& renderData);

		SM_INLINE smMat4& GetProjection() { return m_Projection; }
		SM_INLINE smMat4& GetView() { return m_View; }

		SM_INLINE void SetProjection(const smMat4& projection) { m_Projection = projection; }
		SM_INLINE void SetView(const smMat4& view) { m_View = view; }

		void CreateTexture(mcstring textureName, mbool shouldAutoRelease, muint32 width, muint32 height, muint32 channelCount, const muint8* pixels, mbool hasTransparency, Texture* outTexture);
		void DestroyTexture(Texture* texture);

	private:
		smMat4 m_Projection;
		smMat4 m_View;
		RendererBackend* m_RendererBackend = nullptr;
		mfloat32 m_NearClip;
		mfloat32 m_FarClip;
	};
}

END_NAMESPACE