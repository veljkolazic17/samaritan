#pragma once
#include <defines.hpp>

#include <engine/graphics/renderer/backend/vulkan/vulkandescriptorstate.hpp>

#define SM_VULKAN_OBJECT_SHADER_DESCRIPTOR_COUNT	2

BEGIN_NAMESPACE

namespace Graphics
{
    struct VulkanObjectShaderObjectState
    {
        constexpr muint32 frameCount = 3;
        VkDescriptorSet m_DescriptorSet[frameCount];
        VulkanDescriptorState m_DescriptorStates[SM_VULKAN_OBJECT_SHADER_DESCRIPTOR_COUNT];
    };
}

END_NAMESPACE