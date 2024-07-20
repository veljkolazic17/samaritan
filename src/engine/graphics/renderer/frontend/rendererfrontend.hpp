#pragma once
#include <defines.hpp>
#include <objecttemplates/singleton.hpp>
#include <objecttemplates/shutdownable.hpp>
#include <engine/graphics/renderer/renderertype.inl>
#include <engine/graphics/renderer/backend/rendererbackend.hpp>

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

	private:
		RendererBackend* m_RendererBackend = nullptr;
	};
}

END_NAMESPACE