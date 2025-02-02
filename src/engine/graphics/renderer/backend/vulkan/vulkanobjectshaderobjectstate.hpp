#pragma once
#include <defines.hpp>

#include <engine/graphics/renderer/backend/vulkan/vulkandescriptorstate.hpp>

#define SM_VULKAN_OBJECT_SHADER_DESCRIPTOR_COUNT	2

BEGIN_NAMESPACE

namespace Graphics
{
    struct VulkanObjectShaderObjectState
    {
        VkDescriptorSet m_DescriptorSet[3];
        VulkanDescriptorState m_DescriptorStates[SM_VULKAN_OBJECT_SHADER_DESCRIPTOR_COUNT];
    };
}

END_NAMESPACE