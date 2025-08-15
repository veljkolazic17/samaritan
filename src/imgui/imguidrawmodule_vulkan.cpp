#include <defines.hpp>
#if IMGUI_DISPLAY_ENABLED

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_vulkan.h"

#include <imgui/imguidrawmodule.hpp>

#include <engine/graphics/renderer/backend/vulkan/vulkanbackend.hpp>

BEGIN_NAMESPACE

void ImguiDrawModule::InitWindow_PlatformSpecific()
{
}

void ImguiDrawModule::InitRenderer_PlatformSpecific()
{
}

END_NAMESPACE

#endif