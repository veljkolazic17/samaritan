#pragma once
#include <defines.hpp>

#include <engine/resources/graphics/material.hpp>

#include <objecttemplates/singleton.hpp>
#include <unordered_map>
#include <vector>

BEGIN_NAMESPACE

#define smMaterialSystem()			::samaritan::MaterialSystem::GetInstance()

//TODO : [GRAPHICS] Make reference system for whole engine
struct MaterialReference
{
	muint64 m_RefCount = 0;
	muint32 m_Handle = SM_INVALID_ID;
	mbool m_ShouldAutoRelease = false;
};

struct MaterialSystemConfig
{
	muint32 m_MaxMaterialCount;
};

struct MaterialConfig
{
	muint8 m_Name[SM_MATERIAL_NAME_MAX_LENGTH];
	mbool m_ShouldAutoRelease = false;
	smVec4 m_DiffuseColor;
	muint8 m_DiffuseMapName[SM_TEXTURE_NAME_MAX_LENGTH];
};

class MaterialSystem SINGLETON(MaterialSystem)
{
public:
	SINGLETON_CONSTRUCTOR(MaterialSystem)

	mbool Init(const MaterialSystemConfig & config);
	void Shutdown();

	Material* Acquire(const mstring& name);
	void Release(const mstring& name);

	const Material& GetDefaultMaterial() { return m_DefaultMaterial; }
private:
	void DestroyMaterial(Material * material);
	mbool LoadMaterial(const MaterialConfig & config, Material * material);

	Material* AcquireFromConfig(const MaterialConfig & materialConfig);
	mbool LoadConfigurationFile(mcstring name, MaterialConfig & config);

	Material m_DefaultMaterial;

	MaterialSystemConfig m_Config;

	std::vector<Material> m_Materials;
	std::unordered_map<mstring, MaterialReference> m_MaterialLookup;
};

END_NAMESPACE