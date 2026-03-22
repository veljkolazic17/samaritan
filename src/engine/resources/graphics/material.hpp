#pragma once
#include <defines.hpp>

#include <engine/resources/resource.hpp>
#include <engine/resources/graphics/texture.hpp>
#include <math/vector.hpp>

BEGIN_NAMESPACE

#define SM_DEFAULT_MATERIAL_NAME	"DEFAULT_MATERIAL"
#define SM_MATERIAL_NAME_MAX_LENGTH	1024

struct Material : Resource
{
    using Resource::Resource;

    smstring m_ShaderName;
	smuint32 m_InternalID = SM_INVALID_ID; // Vulkan descriptor set slot
	smVec4 m_DiffuseColor;
	TextureMap m_DiffuseMap;
	TextureMap m_SpecularMap;
	TextureMap m_NormalMap;
	smfloat32 m_Shininess = 32.0f;

    void OnLoad() override;
    void OnUnload() override;

    // Bind this material's per-instance uniforms to the currently active shader.
    void Apply();
};

END_NAMESPACE