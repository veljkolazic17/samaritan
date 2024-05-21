#pragma once

#include <defines.hpp>
#include <math/matrix.hpp>
//TODO : [GRAPHICS] maybe do fdeclare
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
		smMat4 m_Projection;
		smMat4 m_View;
	private:
		smuint8 m_Reserved[NVIDIA_GBO_PADDING];
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
        Geometry* m_Geometry = nullptr;
	};
}

END_NAMESPACE
