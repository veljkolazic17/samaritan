#pragma once
#include <defines.hpp>

#include <engine/memory/memory.hpp>

#include <editor/imgui/imguidrawmodule.hpp>
#include <editor/imgui/imguicentral.hpp>

#ifdef SM_TOOL
#if IMGUI_DISPLAY_ENABLED
BEGIN_NAMESPACE

class ImguiModuleRegistry SINGLETON(ImguiModuleRegistry)
{
public:
    SINGLETON_CONSTRUCTOR(ImguiModuleRegistry);

    void SingletonInit() override;
    void SingletonShutdown() override;

private:

    template <typename T>
    void AddImguiModule();

    std::vector<IImguiModule*> m_Modules;
    ImguiCentral* m_Central = nullptr;
};

template<typename T>
void ImguiModuleRegistry::AddImguiModule()
{
    if(m_Central == nullptr)
    {
        softAssert(false, "ImguiCentral is not initialized!");
        return;
    }

    T* imguiModule = Memory::mnew<T>(Memory::MemoryTag::MEM_EDITOR);
    m_Modules.push_back(imguiModule);

    m_Central->RegisterImguiModule(imguiModule->GetPath(), imguiModule);
}

END_NAMESPACE
#endif
#endif