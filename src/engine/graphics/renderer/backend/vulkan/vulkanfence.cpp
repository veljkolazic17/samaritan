#include <defines.hpp>

#include <engine/graphics/renderer/backend/vulkan/vulkanfence.hpp>

#include <utils/logger/log.hpp>

BEGIN_NAMESPACE

namespace Graphics
{
	void VulkanFence::Create(VkDevice& device, mbool isSignaled, VkAllocationCallbacks* allocator)
	{
		m_IsSignaled = isSignaled;
		VkFenceCreateInfo fenceCreateInfo = { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
		if (isSignaled)
		{
			fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
		}
		VulkanCheckResult(vkCreateFence(device, &fenceCreateInfo, allocator, &m_Handle), "Failed creating Vulkan Fence!");
	}

	void VulkanFence::Destroy(VkDevice& device, VkAllocationCallbacks* allocator)
	{
		if (m_Handle) 
		{
			vkDestroyFence(device, m_Handle, allocator);
			m_Handle = 0;
		}
		m_IsSignaled = false;
	}

	mbool VulkanFence::Wait(VkDevice& device, muint64 timeout)
	{
        constexpr muint32 fenceCount = 1;
        constexpr mbool waitAll = true;

        if (!m_IsSignaled) 
        {
            VkResult result = vkWaitForFences(device, fenceCount, &m_Handle, waitAll, timeout);

            switch (result) 
            {
            case VK_SUCCESS:
            {
                m_IsSignaled = true;
                return true;
            }
            case VK_TIMEOUT:
            {
                LogWarning(LogChannel::Graphics, "VulkanFence::Wait VK_TIMEOUT for %d[ns]", timeout);
                break;
            }
            case VK_ERROR_DEVICE_LOST:
            {
                LogError(LogChannel::Graphics, "VulkanFence::Wait VK_ERROR_DEVICE_LOST");
                break;
            }
            case VK_ERROR_OUT_OF_HOST_MEMORY:
            {
                LogError(LogChannel::Graphics, "VulkanFence::Wait VK_ERROR_OUT_OF_HOST_MEMORY");
                break;
            }
            case VK_ERROR_OUT_OF_DEVICE_MEMORY:
            {
                LogError(LogChannel::Graphics, "VulkanFence::Wait VK_ERROR_OUT_OF_DEVICE_MEMORY");
                break;
            }
            default:
            {
                LogError(LogChannel::Graphics, "VulkanFence::Wait Unexpected error!");
                break;
            }
            }
        }
        else 
        {
            return true;
        }
        return false;
	}

	void VulkanFence::Reset(VkDevice& device)
	{
        constexpr muint32 fenceCount = 1;

        if (m_IsSignaled) 
        {
            VulkanCheckResult(vkResetFences(device, fenceCount, &m_Handle), "Could not reset fence!");
            m_IsSignaled = true;
        }
	}
}

END_NAMESPACE