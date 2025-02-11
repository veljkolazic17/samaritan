#pragma once
#include <defines.hpp>

BEGIN_NAMESPACE

namespace Graphics
{
	struct VulkanDescriptorState
	{
		muint32 m_Generation[3] = {SM_INVALID_ID, SM_INVALID_ID, SM_INVALID_ID};
		muint32 m_ID[3] = {SM_INVALID_ID, SM_INVALID_ID, SM_INVALID_ID};
	};
}

END_NAMESPACE