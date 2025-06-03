#pragma once
#include <defines.hpp>

BEGIN_NAMESPACE

namespace Graphics
{
	struct VulkanDescriptorState
	{
		smuint32 m_Generation[3] = {SM_INVALID_ID, SM_INVALID_ID, SM_INVALID_ID};
		smuint32 m_ID[3] = {SM_INVALID_ID, SM_INVALID_ID, SM_INVALID_ID};
	};
}

END_NAMESPACE