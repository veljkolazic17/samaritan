#pragma once

#include <defines.hpp>
#include <math/matrix.hpp>
// Forward declare Geometry to resolve the issue
namespace samaritan {
   struct Geometry;
}
#include <engine/resources/graphics/texture.hpp>
#include <engine/resources/graphics/material.hpp>
#include <engine/resources/graphics/geometry.hpp>

#define	NVIDIA_GBO_PADDING	128

BEGIN_NAMESPACE

namespace Graphics
{
enum class RendererType
{
    Vulkan,
    DirectX12
};

struct RenderData
{
    Time m_Time;
};

struct GlobalUniformObject
{
    smMat4 m_Projection;      // 64 bytes
    smMat4 m_View;            // 64 bytes
    smVec4 m_AmbientColor;    // 16 bytes
    smVec4 m_DirLightDirection; // 16 bytes
    smVec4 m_DirLightColor;   // 16 bytes
private:
    smuint8 m_Reserved[NVIDIA_GBO_PADDING - 48];
};

struct ObjectUniformObject
{
    smVec4 m_Color;
private:
    smVec4 m_Reserved[3];
};

struct GeometryData
{
    smMat4 m_Model;
    samaritan::Geometry* m_Geometry = nullptr;
};
}

END_NAMESPACE
