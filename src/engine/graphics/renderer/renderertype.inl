#pragma once

#include <defines.hpp>

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
}

END_NAMESPACE
