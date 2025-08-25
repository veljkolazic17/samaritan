#include <defines.hpp>
#include <engine/graphics/renderer/backend/vulkan/vulkantypes.hpp>

#include <engine/resources/graphics/shader.hpp>

BEGIN_NAMESPACE

namespace Graphics
{
    namespace Vulkan::Utils
    {
        smbool IsResultSuccess(VkResult result)
        {
            switch (result)
            {
                // Success Codes
            default:
            case VK_SUCCESS:
            case VK_NOT_READY:
            case VK_TIMEOUT:
            case VK_EVENT_SET:
            case VK_EVENT_RESET:
            case VK_INCOMPLETE:
            case VK_SUBOPTIMAL_KHR:
            case VK_THREAD_IDLE_KHR:
            case VK_THREAD_DONE_KHR:
            case VK_OPERATION_DEFERRED_KHR:
            case VK_OPERATION_NOT_DEFERRED_KHR:
            case VK_PIPELINE_COMPILE_REQUIRED_EXT:
            {
                return true;
            }
            // Error codes
            case VK_ERROR_OUT_OF_HOST_MEMORY:
            case VK_ERROR_OUT_OF_DEVICE_MEMORY:
            case VK_ERROR_INITIALIZATION_FAILED:
            case VK_ERROR_DEVICE_LOST:
            case VK_ERROR_MEMORY_MAP_FAILED:
            case VK_ERROR_LAYER_NOT_PRESENT:
            case VK_ERROR_EXTENSION_NOT_PRESENT:
            case VK_ERROR_FEATURE_NOT_PRESENT:
            case VK_ERROR_INCOMPATIBLE_DRIVER:
            case VK_ERROR_TOO_MANY_OBJECTS:
            case VK_ERROR_FORMAT_NOT_SUPPORTED:
            case VK_ERROR_FRAGMENTED_POOL:
            case VK_ERROR_SURFACE_LOST_KHR:
            case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
            case VK_ERROR_OUT_OF_DATE_KHR:
            case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
            case VK_ERROR_INVALID_SHADER_NV:
            case VK_ERROR_OUT_OF_POOL_MEMORY:
            case VK_ERROR_INVALID_EXTERNAL_HANDLE:
            case VK_ERROR_FRAGMENTATION:
            case VK_ERROR_INVALID_DEVICE_ADDRESS_EXT:
                //case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS:
            case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:
            case VK_ERROR_UNKNOWN:
            {
                return false;
            }
            }
        }
        const char* ResultToString(VkResult result) {
            // Success Codes
            switch (result) {
            default:
            case VK_SUCCESS:
                return "VK_SUCCESS";
            case VK_NOT_READY:
                return "VK_NOT_READY";
            case VK_TIMEOUT:
                return "VK_TIMEOUT";
            case VK_EVENT_SET:
                return "VK_EVENT_SET";
            case VK_EVENT_RESET:
                return "VK_EVENT_RESET";
            case VK_INCOMPLETE:
                return "VK_INCOMPLETE";
            case VK_SUBOPTIMAL_KHR:
                return "VK_SUBOPTIMAL_KHR";
            case VK_THREAD_IDLE_KHR:
                return "VK_THREAD_IDLE_KHR";
            case VK_THREAD_DONE_KHR:
                return "VK_THREAD_DONE_KHR";
            case VK_OPERATION_DEFERRED_KHR:
                return "VK_OPERATION_DEFERRED_KHR";
            case VK_OPERATION_NOT_DEFERRED_KHR:
                return "VK_OPERATION_NOT_DEFERRED_KHR";
            case VK_PIPELINE_COMPILE_REQUIRED_EXT:
                return "VK_PIPELINE_COMPILE_REQUIRED_EXT";
                // Error codes
            case VK_ERROR_OUT_OF_HOST_MEMORY:
                return "VK_ERROR_OUT_OF_HOST_MEMORY";
            case VK_ERROR_OUT_OF_DEVICE_MEMORY:
                return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
            case VK_ERROR_INITIALIZATION_FAILED:
                return "VK_ERROR_INITIALIZATION_FAILED";
            case VK_ERROR_DEVICE_LOST:
                return "VK_ERROR_DEVICE_LOST";
            case VK_ERROR_MEMORY_MAP_FAILED:
                return "VK_ERROR_MEMORY_MAP_FAILED";
            case VK_ERROR_LAYER_NOT_PRESENT:
                return "VK_ERROR_LAYER_NOT_PRESENT";
            case VK_ERROR_EXTENSION_NOT_PRESENT:
                return "VK_ERROR_EXTENSION_NOT_PRESENT";
            case VK_ERROR_FEATURE_NOT_PRESENT:
                return "VK_ERROR_FEATURE_NOT_PRESENT";
            case VK_ERROR_INCOMPATIBLE_DRIVER:
                return "VK_ERROR_INCOMPATIBLE_DRIVER";
            case VK_ERROR_TOO_MANY_OBJECTS:
                return "VK_ERROR_TOO_MANY_OBJECTS";
            case VK_ERROR_FORMAT_NOT_SUPPORTED:
                return "VK_ERROR_FORMAT_NOT_SUPPORTED";
            case VK_ERROR_FRAGMENTED_POOL:
                return "VK_ERROR_FRAGMENTED_POOL";
            case VK_ERROR_SURFACE_LOST_KHR:
                return "VK_ERROR_SURFACE_LOST_KHR";
            case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
                return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
            case VK_ERROR_OUT_OF_DATE_KHR:
                return "VK_ERROR_OUT_OF_DATE_KHR";
            case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
                return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
            case VK_ERROR_INVALID_SHADER_NV:
                return "VK_ERROR_INVALID_SHADER_NV";
            case VK_ERROR_OUT_OF_POOL_MEMORY:
                return "VK_ERROR_OUT_OF_POOL_MEMORY";
            case VK_ERROR_INVALID_EXTERNAL_HANDLE:
                return "VK_ERROR_INVALID_EXTERNAL_HANDLE";
            case VK_ERROR_FRAGMENTATION:
                return "VK_ERROR_FRAGMENTATION";
            case VK_ERROR_INVALID_DEVICE_ADDRESS_EXT:
                return "VK_ERROR_INVALID_DEVICE_ADDRESS_EXT";
                // NOTE: Same as above
                //case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS:
                //    return "VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS";
            case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:
                return "VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT";
            case VK_ERROR_UNKNOWN:
                return "VK_ERROR_UNKNOWN";
            }
        }
        VkFormat ShaderDataTypeToVkFormat(ShaderDataType type)
        {
            switch (type)
            {
            case ShaderDataType::FLOAT32:    return VK_FORMAT_R32_SFLOAT;
            case ShaderDataType::FLOAT32_2:  return VK_FORMAT_R32G32_SFLOAT;
            case ShaderDataType::FLOAT32_3:  return VK_FORMAT_R32G32B32_SFLOAT;
            case ShaderDataType::FLOAT32_4:  return VK_FORMAT_R32G32B32A32_SFLOAT;
            case ShaderDataType::MATRIX_4:   return VK_FORMAT_R32G32B32A32_SFLOAT; // Note: Matrices are typically represented as arrays of vectors
            case ShaderDataType::INT8:       return VK_FORMAT_R8_SINT;
            case ShaderDataType::UINT8:      return VK_FORMAT_R8_UINT;
            case ShaderDataType::INT16:      return VK_FORMAT_R16_SINT;
            case ShaderDataType::UINT16:     return VK_FORMAT_R16_UINT;
            case ShaderDataType::INT32:      return VK_FORMAT_R32_SINT;
            case ShaderDataType::UINT32:     return VK_FORMAT_R32_UINT;
            case ShaderDataType::SAMPLER:    return VK_FORMAT_UNDEFINED; // Samplers don't have a direct format representation
            default:
                softAssert(false, "Unknown shader data type!");
                return VK_FORMAT_UNDEFINED;
            }
        }
    }
}

END_NAMESPACE