#include <engine/graphics/renderer/backend/vulkan/vulkandevice.hpp>
#include <engine/graphics/renderer/backend/vulkan/vulkanimage.hpp>
#include <engine/graphics/renderer/backend/vulkan/vulkantypes.inl>

#include <utils/logger/log.hpp>

#include "vulkanbackend.hpp"

BEGIN_NAMESPACE
	namespace Graphics
{
    void VulkanImage::CreateImage
    (
        const VkPhysicalDevice& physicalDevice,
        const VkDevice& logicalDevice,
        const VkAllocationCallbacks* allocator,
        VkImageType imageType,
        muint32 width,
        muint32 height,
        VkFormat& format,
        VkImageTiling tiling,
        VkImageUsageFlags usage,
        VkMemoryPropertyFlags memoryFlags,
        mbool shouldCreateView,
        VkImageAspectFlags viewAspectFlags,
        VulkanImage* outImage
    )
    {
        // Copy params
        outImage->m_Width = width;
        outImage->m_Height = height;

        // Creation info.
        VkImageCreateInfo imageCreateInfo = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
        imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
        imageCreateInfo.extent.width = width;
        imageCreateInfo.extent.height = height;
        imageCreateInfo.extent.depth = 1;                           // TODO: Support configurable depth.
        imageCreateInfo.mipLevels = 4;                              // TODO: Support mip mapping
        imageCreateInfo.arrayLayers = 1;                            // TODO: Support number of layers in the image.
        imageCreateInfo.format = format;
        imageCreateInfo.tiling = tiling;
        imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageCreateInfo.usage = usage;
        imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;            // TODO: Configurable sample count.
        imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;    // TODO: Configurable sharing mode.

        VulkanCheckResult(vkCreateImage(logicalDevice, &imageCreateInfo, allocator, &outImage->m_Handle), "Error creating image!");

        // Query memory requirements.
        VkMemoryRequirements memoryReq;
        vkGetImageMemoryRequirements(logicalDevice, outImage->m_Handle, &memoryReq);

        VkPhysicalDeviceMemoryProperties memoryProperties;
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);

        mint32 memtype = VulkanDevice::FindMemoryIndex(physicalDevice, memoryReq.memoryTypeBits, memoryFlags);
        if (memtype == -1)
        {
            hardAssert(false, "Required memory type not found.");
        }

        // Allocate memory
        VkMemoryAllocateInfo memAllocateInfo = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
        memAllocateInfo.allocationSize = memoryReq.size;
        memAllocateInfo.memoryTypeIndex = memtype;
        VulkanCheckResult(vkAllocateMemory(logicalDevice, &memAllocateInfo, allocator, &outImage->m_Memory), "Error allocating memory for image!");

        // Bind the memory
        VulkanCheckResult(vkBindImageMemory(logicalDevice, outImage->m_Handle, outImage->m_Memory, 0), "Error binding image memory!");  // TODO: configurable memory offset.

        // Create view
        if (shouldCreateView) 
        {
            CreateImageView(logicalDevice, allocator, format, viewAspectFlags, outImage);
        }
    }

    void VulkanImage::CreateImageView
    (
        const VkDevice& logicalDevice,
        const VkAllocationCallbacks* allocator,
        VkFormat& format,
        VkImageAspectFlags aspectFlags,
        VulkanImage* image
    )
    {
        VkImageViewCreateInfo viewCreateInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
        viewCreateInfo.image = image->m_Handle;
        viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;  // TODO: Make configurable.
        viewCreateInfo.format = format;
        viewCreateInfo.subresourceRange.aspectMask = aspectFlags;

        // TODO: Make configurable
        viewCreateInfo.subresourceRange.baseMipLevel = 0;
        viewCreateInfo.subresourceRange.levelCount = 1;
        viewCreateInfo.subresourceRange.baseArrayLayer = 0;
        viewCreateInfo.subresourceRange.layerCount = 1;

        VulkanCheckResult(vkCreateImageView(logicalDevice, &viewCreateInfo, allocator, &image->m_View), "Error creating image view!");
    }

    void VulkanImage::DeleteImage
    (
        const VkDevice& logicalDevice,
        const VkAllocationCallbacks* allocator,
        VulkanImage& image
    )
    {
        vkDestroyImageView(logicalDevice, image.GetImageView(), allocator);
        vkFreeMemory(logicalDevice, image.GetMemory(), allocator);
        vkDestroyImage(logicalDevice, image.GetImage(), allocator);
    }

    void VulkanImage::TransitionLayout(VulkanCommandBuffer* commandBuffer, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
    {
        VkImageMemoryBarrier barrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;

        const muint32 graphicsQueueIndex = g_VulkanRenderer->GetVulkanDevice().m_QueuesInfo.m_GraphicsIndex;

        barrier.srcQueueFamilyIndex = graphicsQueueIndex;
        barrier.dstQueueFamilyIndex = graphicsQueueIndex;
        barrier.image = m_Handle;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;

        VkPipelineStageFlags sourceStage;
        VkPipelineStageFlags destStage;

        if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) 
        {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        }
        else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
        {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }
        else 
        {
            softAssert(false, "Transition layout not supported!");
            return;
        }

        vkCmdPipelineBarrier
    	(
            commandBuffer->GetHandle(),
            sourceStage, 
            destStage,
            0,
            0, 0,
            0, 0,
            1, &barrier
        );
    }

    void VulkanImage::CopyImageFromBuffer(VulkanCommandBuffer* commandBuffer, VkBuffer buffer)
    {
        // Region to copy
        VkBufferImageCopy region;
        smZero(&region, sizeof(VkBufferImageCopy));
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;

        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;

        region.imageExtent.width = m_Width;
        region.imageExtent.height = m_Height;
        //TODO : [GRAPHICS] make this configurable when we want to use 3D textures
        region.imageExtent.depth = 1;

        constexpr muint32 regionCount = 1;

        vkCmdCopyBufferToImage
    	(
            commandBuffer->GetHandle(),
            buffer,
            m_Handle,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            regionCount,
            &region
        );
    }
}

END_NAMESPACE