#pragma once
#ifdef SM_TOOL
#include <defines.hpp>
#include <objecttemplates/singleton.hpp>
#include <math/matrix.hpp>
#include <math/math.hpp>

#if IMGUI_DISPLAY_ENABLED
#include <editor/imgui/imguicentral.hpp>
#endif

#define smTransformDebug() ::samaritan::TransformDebug::GetInstance()

BEGIN_NAMESPACE

class TransformDebug SINGLETON(TransformDebug) IMGUI_DISPLAY(, public IImguiModule)
{
public:
    SINGLETON_CONSTRUCTOR(TransformDebug)
    void SingletonInit() override;

#if IMGUI_DISPLAY_ENABLED
    void DrawImgui() override;
#endif

    SM_INLINE smMat4 GetModelMatrix() const
    {
        return smMat4::RotationEuler(
            Math::Deg2Rad(m_Rotation[0]),
            Math::Deg2Rad(m_Rotation[1]),
            Math::Deg2Rad(m_Rotation[2])
        );
    }

private:
    smfloat32 m_Rotation[3] = { 0.0f, 0.0f, 0.0f };
};

END_NAMESPACE
#endif // SM_TOOL
