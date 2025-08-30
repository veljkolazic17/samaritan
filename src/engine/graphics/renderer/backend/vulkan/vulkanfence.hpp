#pragma once
#include <defines.hpp>
#include <engine/graphics/renderer/backend/vulkan/vulkantypes.hpp>

#include <objecttemplates/shutdownable.hpp>

BEGIN_NAMESPACE

namespace Graphics
{
	class VulkanFence
	{
	public:
		void Create(VkDevice& device, smbool isSignaled, VkAllocationCallbacks* allocator);
		void Destroy(VkDevice& device, VkAllocationCallbacks* allocator);

		SM_INLINE VkFence& GetHandle() { return m_Handle; }
		SM_INLINE void SetHandle(const VkFence& fence) { m_Handle = fence; }
		SM_INLINE smbool GetIsSignaled() { return m_IsSignaled; }
		SM_INLINE void GetIsSignaled(smbool isSignaled) { m_IsSignaled = isSignaled; }

		// @param timeout = timeout in nanoseconds
		smbool Wait(VkDevice& device, smuint64 timeout);
		void Reset(VkDevice& device);
	private:
		VkFence m_Handle = 0;
		smbool m_IsSignaled = false;
	};
}

END_NAMESPACE