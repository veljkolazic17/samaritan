#pragma once
#include <defines.hpp>

#include <imguitools/imguimoduleregistry.hpp>

#ifdef SM_TOOL
#if IMGUI_DISPLAY_ENABLED
#include <imguitools/entity/entityviewer.hpp>
#include <imguitools/world/worldviewer.hpp>

BEGIN_NAMESPACE

void ImguiModuleRegistry::SingletonInit()
{
    m_Central = &smImguiCentral();

    AddImguiModule<EntityViewer>();
    AddImguiModule<WorldViewer>();
}

void ImguiModuleRegistry::SingletonShutdown()
{

}

END_NAMESPACE
#endif
#endif