#pragma once
#include <defines.hpp>
#include <engine/graphics/renderer/backend/vulkan/vulkantypes.inl>

BEGIN_NAMESPACE

namespace Graphics
{
	class VulkanImage
	{
	public:
        static void CreateImage
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
        );

        static void CreateImageView
        (
            const VkDevice& logicalDevice,
            const VkAllocationCallbacks* allocator,
            VkFormat& format,
            VkImageAspectFlags aspectFlags,
            VulkanImage* image
        );

        static void DeleteImage
        (
            const VkDevice& logicalDevice,
            const VkAllocationCallbacks* allocator,
            VulkanImage& image
        );

        SM_INLINE VkImage& GetImage() { return m_Handle; }
        SM_INLINE VkImageView& GetImageView() { return m_View; }
        SM_INLINE VkDeviceMemory& GetMemory() { return m_Memory; }
        SM_INLINE muint32 GetWidth() { return m_Width; }
        SM_INLINE muint32 GetHeight() { return m_Height; }
    private:
        VkImage m_Handle;
        VkDeviceMemory m_Memory;
        VkImageView m_View;
        muint32 m_Width;
        muint32 m_Height;
	};
}

END_NAMESPACE