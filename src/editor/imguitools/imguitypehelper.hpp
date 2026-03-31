#pragma once
#include <defines.hpp>

#ifdef SM_TOOL
#if IMGUI_DISPLAY_ENABLED

#include <math/vector.hpp>

#include "imgui.h"

BEGIN_NAMESPACE

// Not good
namespace TypeHelper
{
    void DrawType(const char* memberName, const char* typeName, void* data)
    {
        smstring type = typeName;

        if (type == typeid(float).name())
        {
            constexpr smfloat32 speed = 0.1;
            ImGui::DragFloat(memberName, static_cast<float*>(data), speed);
        }
        else if (type == typeid(smVec3).name())
        {
            constexpr smfloat32 speed = 0.1;
            ImGui::DragFloat3(memberName, &static_cast<smVec3*>(data)->m_X, speed);
        }
    }
}

END_NAMESPACE
#endif
#endif