#pragma once
#include <defines.hpp>
#if IMGUI_DISPLAY_ENABLED

#include <objecttemplates/singleton.hpp>

#define smImguiDrawModule() ::samaritan::ImguiDrawModule::GetInstance()

BEGIN_NAMESPACE

class ImguiDrawModule SINGLETON(ImguiDrawModule)
{
public:
    SINGLETON_CONSTRUCTOR(ImguiDrawModule);

    void Init();
private:
    void InitRenderer_PlatformSpecific();
    void InitWindow_PlatformSpecific();
    void InitInputs_PlatformSpecific();
};

END_NAMESPACE
#endif