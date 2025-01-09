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

        void Create(VulkanRenderer* renderer, muint32 size, VkBufferUsageFlagBits usage,muint32 memoryPropertyFlags, mbool bindOnCreate);
        void Destroy();
        mbool Resize(muint32 size, VkQueue queue, VkCommandPool pool);
        void Bind(muint64 offset);
        void* LockMemory(muint64 offset, muint64 size, muint32 flags);
        void UnlockMemory();
        void LoadData(muint64 offset, muint64 size, muint32 flags, void* data);
        void CopyTo(VkCommandPool pool, VkFence fence, VkQueue queue, VkBuffer destination, muint64 sourceOffset, muint64 destinationOffset, muint64 size);

	private:
        muint64 m_Size;
        VkBuffer m_Handle;
        VkBufferUsageFlagBits m_Usage;
        mbool m_IsLocked;
        VkDeviceMemory m_Memory;
        mint32 m_MemoryIndex;
        muint32 m_MemoryPropertyFlags;

        VulkanRenderer* m_Renderer;
	};
}

END_NAMESPACE