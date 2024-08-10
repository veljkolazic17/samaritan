#include <engine/graphics/renderer/backend/vulkan/vulkanimage.hpp>
#include <engine/graphics/renderer/backend/vulkan/vulkantypes.inl>

#include <utils/logger/log.hpp>

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
        
        //SUS
        mint32 memtype = -1;
        for (muint32 i = 0; i < memoryProperties.memoryTypeCount; ++i)
        {
            // Check each memory type to see if its bit is set to 1.
            if (memoryReq.memoryTypeBits & (1 << i) && (memoryProperties.memoryTypes[i].propertyFlags & memoryFlags) == memoryFlags)
            {
                memtype = i;
                break;
            }
        }
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
}

END_NAMESPACE