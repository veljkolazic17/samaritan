#pragma once
#include <defines.hpp>

#include <math/vector.hpp>
#include <engine/resources/graphics/texture.hpp>

BEGIN_NAMESPACE

#define SM_DEFAULT_MATERIAL_NAME	"DEFAULT_MATERIAL"
#define SM_MATERIAL_NAME_MAX_LENGTH	1024

struct Material
{
	smuint8 m_Name[SM_MATERIAL_NAME_MAX_LENGTH];
    smstring m_ShaderName;
	smuint32 m_ID = SM_INVALID_ID;
	smuint32 m_Generation = SM_INVALID_ID;
	smuint32 m_InternalID = SM_INVALID_ID;
	smVec4 m_DiffuseColor;
	//Expect usage for texture to be TEXTURE_USAGE_MAP_DIFFUSE
	TextureMap m_DiffuseMap;
	//Expect usage for texture to be TEXTURE_USAGE_MAP_SPECULAR
	TextureMap m_SpecularMap;
	//Expect usage for texture to be TEXTURE_USAGE_MAP_NORMAL
	TextureMap m_NormalMap;
	smfloat32 m_Shininess = 32.0f;
};

END_NAMESPACE