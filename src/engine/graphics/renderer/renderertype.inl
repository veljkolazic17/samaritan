#pragma once

#include <defines.hpp>
#include <math/matrix.hpp>

#define	NVIDIA_GBO_PADDING	128

BEGIN_NAMESPACE

namespace Graphics
{
	enum class RendererType
	{
		Vulkan,
		DirectX12
	};

	struct RenderData
	{
		Time m_Time;
	};

	
	struct GlobalUniformObject
	{
		smMat4 m_Projection;
		smMat4 m_View;
		muint8 m_Reserved[NVIDIA_GBO_PADDING];
	};
}

END_NAMESPACE
