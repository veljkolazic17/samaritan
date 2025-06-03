#pragma once
#include <defines.hpp>

#include <math/vector.hpp>
#include <engine/resources/graphics/texture.hpp>

BEGIN_NAMESPACE

#define SM_DEFAULT_MATERIAL_NAME	"DEFAULT"
#define SM_MATERIAL_NAME_MAX_LENGTH	1024

struct Material
{
	smuint8 m_Name[SM_MATERIAL_NAME_MAX_LENGTH];
	smuint32 m_ID = SM_INVALID_ID;
	smuint32 m_Generation = SM_INVALID_ID;
	smuint32 m_InternalID = SM_INVALID_ID;
	smVec4 m_DiffuseColor;
	//Expect usage for texture to be TEXTURE_USAGE_MAP_DIFFUSE
	TextureMap m_DiffuseMap;
};

END_NAMESPACE