#pragma once
#include <defines.hpp>
#include <utils/asserts/assert.hpp>
#include <vulkan/vulkan.h>


#define VulkanCheckResult(result, message)					\
if (result != VK_SUCCESS) {									\
hardAssert(false, message);									\
}
