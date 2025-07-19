#include <engine/graphics/renderer/backend/vulkan/vulkanbackend.hpp>
#include <engine/graphics/renderer/backend/vulkan/vulkanbuffer.hpp>
#include <engine/graphics/renderer/backend/vulkan/vulkandevice.hpp>
#include <engine/graphics/renderer/backend/vulkan/vulkancommandbuffer.hpp>

#include <engine/memory/memory.hpp>

BEGIN_NAMESPACE

namespace Graphics
{
    void VulkanBuffer::Create(smuint32 size, VkBufferUsageFlagBits usage, smuint32 memoryPropertyFlags, smbool bindOnCreate)
    {
        m_Memory = 0;
        m_Size = size;
        m_Usage = usage;
        m_MemoryPropertyFlags = memoryPropertyFlags;

        VkBufferCreateInfo bufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;  //Only used in one queue.

        VulkanCheckResult(vkCreateBuffer(g_VulkanRenderer->GetVulkanDevice().m_LogicalDevice, &bufferInfo, g_VulkanRenderer->GetAllocator(), &m_Handle), "Could not create vulkan buffer!");

        // Gather memory requirements.
        VkMemoryRequirements requirements;
        vkGetBufferMemoryRequirements(g_VulkanRenderer->GetVulkanDevice().m_LogicalDevice, m_Handle, &requirements);
        m_MemoryIndex = VulkanDevice::FindMemoryIndex(g_VulkanRenderer->GetVulkanDevice().m_PhysicalDevice, requirements.memoryTypeBits, m_MemoryPropertyFlags);
        if (m_MemoryIndex == -1) 
        {
            hardAssert(false, "Required memory type not found.");
        }

        // Allocate memory info
        VkMemoryAllocateInfo allocatInfo = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
        allocatInfo.allocationSize = requirements.size;
        allocatInfo.memoryTypeIndex = m_MemoryIndex;

        // Allocate the memory.
        VkResult result = vkAllocateMemory(g_VulkanRenderer->GetVulkanDevice().m_LogicalDevice, &allocatInfo, g_VulkanRenderer->GetAllocator(), &m_Memory);

        if (result != VK_SUCCESS) 
        {
            hardAssert(false, "Unable to create vulkan buffer. Memory allocation failed: %i", result);
        }

        if (bindOnCreate) 
        {
            Bind(0);
        }
    }

    void VulkanBuffer::Destroy()
    {
        if (m_Memory)
        {
            vkFreeMemory(g_VulkanRenderer->GetVulkanDevice().m_LogicalDevice, m_Memory, g_VulkanRenderer->GetAllocator());
            m_Memory = 0;
        }
        if (m_Handle) 
        {
            vkDestroyBuffer(g_VulkanRenderer->GetVulkanDevice().m_LogicalDevice, m_Handle, g_VulkanRenderer->GetAllocator());
            m_Handle = 0;
        }

        m_Size = 0;
        m_Usage = (VkBufferUsageFlagBits)0;
        m_IsLocked = false;
    }

    smbool VulkanBuffer::Resize(smuint32 size, VkQueue queue, VkCommandPool pool)
    {
        // Create new buffer.
        VkBufferCreateInfo bufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
        bufferInfo.size = size;
        bufferInfo.usage = m_Usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;  //Only used in one queue.

        VkBuffer newBuffer;
        VulkanCheckResult(vkCreateBuffer(g_VulkanRenderer->GetVulkanDevice().m_LogicalDevice, &bufferInfo, g_VulkanRenderer->GetAllocator(), &newBuffer), "Could not create new buffer!");

        // Gather memory requirements.
        VkMemoryRequirements requirements;
        vkGetBufferMemoryRequirements(g_VulkanRenderer->GetVulkanDevice().m_LogicalDevice, newBuffer, &requirements);

        // Allocate memory info
        VkMemoryAllocateInfo allocateInfo = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
        allocateInfo.allocationSize = requirements.size;
        allocateInfo.memoryTypeIndex = m_MemoryIndex;

        // Allocate the memory.
        VkDeviceMemory newMemory;
        VkResult result = vkAllocateMemory(g_VulkanRenderer->GetVulkanDevice().m_LogicalDevice, &allocateInfo, g_VulkanRenderer->GetAllocator(), &newMemory);
        if (result != VK_SUCCESS) 
        {
            hardAssert(false, "Unable to create vulkan buffer. Memory allocation failed: %i", result);
        }

        // Bind the new buffer's memory
        VulkanCheckResult(vkBindBufferMemory(g_VulkanRenderer->GetVulkanDevice().m_LogicalDevice, newBuffer, newMemory, 0), "Could not bind buffer!");

        // Copy over the data
        CopyTo(pool, 0, queue, newBuffer, 0, 0, m_Size);

        // Make sure anything potentially using these is finished.
        vkDeviceWaitIdle(g_VulkanRenderer->GetVulkanDevice().m_LogicalDevice);

        // Destroy the old
        if (m_Memory) 
        {
            vkFreeMemory(g_VulkanRenderer->GetVulkanDevice().m_LogicalDevice, m_Memory, g_VulkanRenderer->GetAllocator());
        }

        if (m_Handle) 
        {
            vkDestroyBuffer(g_VulkanRenderer->GetVulkanDevice().m_LogicalDevice, m_Handle, g_VulkanRenderer->GetAllocator());
        }

        // Set new properties
        m_Size = size;
        m_Memory = newMemory;
        m_Handle = newBuffer;

        return true;
    }

    void VulkanBuffer::Bind(smuint64 offset)
    {
        VulkanCheckResult(vkBindBufferMemory(g_VulkanRenderer->GetVulkanDevice().m_LogicalDevice, m_Handle, m_Memory, offset), "Could not bind vulkan buffer!");
    }

    void* VulkanBuffer::LockMemory(smuint64 offset, smuint64 size, smuint32 flags)
    {
        void* data;
        VulkanCheckResult(vkMapMemory(g_VulkanRenderer->GetVulkanDevice().m_LogicalDevice, m_Memory, offset, size, flags, &data), "Could not lock memory!");
        return data;
    }

    void VulkanBuffer::UnlockMemory()
    {
        vkUnmapMemory(g_VulkanRenderer->GetVulkanDevice().m_LogicalDevice, m_Memory);
    }

    void VulkanBuffer::LoadData(smuint64 offset, smuint64 size, smuint32 flags, const void* data) 
    {
        void* mdata;
        VulkanCheckResult(vkMapMemory(g_VulkanRenderer->GetVulkanDevice().m_LogicalDevice, m_Memory, offset, size, flags, &mdata), "Could not load data!");
        smCopy(data, mdata, size);
        vkUnmapMemory(g_VulkanRenderer->GetVulkanDevice().m_LogicalDevice, m_Memory);
    }

    void VulkanBuffer::CopyTo(VkCommandPool pool, VkFence fence, VkQueue queue, VkBuffer destination, smuint64 sourceOffset, smuint64 destinationOffset, smuint64 size)
    {
        vkQueueWaitIdle(queue);
        // Create a one-time-use command buffer.
        VulkanCommandBuffer tempcmdbuff;
        tempcmdbuff.BeginSingleUseBuffer(g_VulkanRenderer->GetVulkanDevice().m_LogicalDevice, pool);

        // Prepare the copy command and add it to the command buffer.
        VkBufferCopy copyRegion;
        copyRegion.srcOffset = sourceOffset;
        copyRegion.dstOffset = destinationOffset;
        copyRegion.size = size;

        vkCmdCopyBuffer(tempcmdbuff.GetHandle(), m_Handle, destination, 1, &copyRegion);

        // Submit the buffer for execution and wait for it to complete.
        tempcmdbuff.EndSingleUseBuffer(g_VulkanRenderer->GetVulkanDevice().m_LogicalDevice, pool, queue);
    }

    void VulkanBuffer::Free(smuint32 offset, smuint32 size)
    {
        //TODO: [GRAPHICS] implement this
    }
}

END_NAMESPACE