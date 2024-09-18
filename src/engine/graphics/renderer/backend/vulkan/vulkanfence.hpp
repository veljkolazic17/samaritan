#include <defines.hpp>
#include <engine/graphics/renderer/backend/vulkan/vulkantypes.inl>

#include <objecttemplates/shutdownable.hpp>

BEGIN_NAMESPACE

namespace Graphics
{
	class VulkanFence
	{
	public:
		void Create(VkDevice& device, mbool isSignaled, VkAllocationCallbacks* allocator);
		void Destroy(VkDevice& device, VkAllocationCallbacks* allocator);

		SM_INLINE VkFence& GetHandle() { return m_Handle; }
		SM_INLINE void SetHandle(const VkFence& fence) { m_Handle = fence; }
		SM_INLINE mbool GetIsSignaled() { return m_IsSignaled; }
		SM_INLINE void GetIsSignaled(mbool isSignaled) { m_IsSignaled = isSignaled; }

		// @param timeout = timeout in nanoseconds
		mbool Wait(VkDevice& device, muint64 timeout);
		void Reset(VkDevice& device);
	private:
		VkFence m_Handle = 0;
		mbool m_IsSignaled = false;
	};
}

END_NAMESPACE