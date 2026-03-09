#pragma once
#ifdef SM_TOOL
#include <defines.hpp>
#include <objecttemplates/singleton.hpp>
#include <math/vector.hpp>

#if IMGUI_DISPLAY_ENABLED
#include <imgui/imguicentral.hpp>
#endif

#define smLightingDebug() ::samaritan::LightingDebug::GetInstance()

BEGIN_NAMESPACE

class LightingDebug SINGLETON(LightingDebug) IMGUI_DISPLAY(, public IImguiModule)
{
public:
    SINGLETON_CONSTRUCTOR(LightingDebug)
    void SingletonInit() override;

#if IMGUI_DISPLAY_ENABLED
    void DrawImgui() override;
#endif

    SM_INLINE const smVec4& GetAmbientColor() const { return m_AmbientColor; }

private:
    smVec4 m_AmbientColor = smVec4{ 0.1f, 0.1f, 0.1f, 1.0f };
};

END_NAMESPACE
#endif // SM_TOOL
