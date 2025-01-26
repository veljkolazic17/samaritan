#pragma once
#include <defines.hpp>
#include <engine/graphics/renderer/renderertype.inl>

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

		SM_INLINE muint64 GetFrameCount() { return m_FrameCount; }
		SM_INLINE muint64 SetFrameCount(muint64 frameCount) { m_FrameCount = frameCount; }
		SM_INLINE muint64 IncrementFrameCount() { return ++m_FrameCount; }

		virtual void Resize(muint32 width, muint32 heigth) = 0;
		virtual mbool BeginFrame(Time time) = 0;
		virtual mbool EndFrame(Time time) = 0;
		virtual void UpdateGlobalState(smMat4 projection, smMat4 view, smVec3 viewPosition, smVec4 ambientColor, mint32 mode) = 0;
		virtual void UpdateObject(smMat4 model) = 0;

	protected:
		RendererType m_RendererType;
		mbool m_Initialized = false;
		muint64 m_FrameCount = 0;
	};
}

END_NAMESPACE

#define DEFINE_RENDERER(rendererType)				\
	RendererType GetRendererType() override			\
	{                                           	\
		return rendererType;            			\
	}