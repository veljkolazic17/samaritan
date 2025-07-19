#pragma once
#include <defines.hpp>

#include <engine/graphics/renderer/backend/vulkan/vulkantypes.inl>

BEGIN_NAMESPACE

namespace Graphics
{
    class VulkanRenderer;
}

namespace Graphics
{
	class VulkanBuffer
	{
    public:
        SM_INLINE VkBuffer& GetHandle() { return m_Handle; }
        SM_INLINE const VkBuffer& GetHandle() const { return m_Handle; }

        void Create(smuint32 size, VkBufferUsageFlagBits usage,smuint32 memoryPropertyFlags, smbool bindOnCreate);
        void Destroy();
        smbool Resize(smuint32 size, VkQueue queue, VkCommandPool pool);
        void Bind(smuint64 offset);
        void* LockMemory(smuint64 offset, smuint64 size, smuint32 flags);
        void UnlockMemory();
        void LoadData(smuint64 offset, smuint64 size, smuint32 flags, const void* data);
        void CopyTo(VkCommandPool pool, VkFence fence, VkQueue queue, VkBuffer destination, smuint64 sourceOffset, smuint64 destinationOffset, smuint64 size);
        void Free(smuint32 offset, smuint32 size);

	private:
        smuint64 m_Size;
        VkBuffer m_Handle;
        VkBufferUsageFlagBits m_Usage;
        smbool m_IsLocked;
        VkDeviceMemory m_Memory;
        smint32 m_MemoryIndex;
        smuint32 m_MemoryPropertyFlags;
	};
}

END_NAMESPACE