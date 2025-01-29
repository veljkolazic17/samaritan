#pragma once
#include <defines.hpp>

BEGIN_NAMESPACE

namespace Graphics
{
	struct VulkanDescriptorState
	{
		constexpr muint32 frameCount = 3;
		muint32 m_Generation[frameCount];
	};
}

END_NAMESPACE