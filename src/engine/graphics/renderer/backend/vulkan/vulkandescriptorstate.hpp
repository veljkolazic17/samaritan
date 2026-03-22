#pragma once
#include <defines.hpp>

BEGIN_NAMESPACE

namespace Graphics
{
	struct VulkanDescriptorState
	{
		smuint64 m_ResourceId[3] = {SM_INVALID_ID, SM_INVALID_ID, SM_INVALID_ID};
	};
}

END_NAMESPACE
