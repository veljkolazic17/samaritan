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
	smuint64 m_RefCount = 0;
	smuint32 m_Handle = SM_INVALID_ID;
	smbool m_ShouldAutoRelease = false;
};

struct MaterialSystemConfig
{
	smuint32 m_MaxMaterialCount;
};

struct MaterialConfig
{
	smuint8 m_Name[SM_MATERIAL_NAME_MAX_LENGTH];
	smbool m_ShouldAutoRelease = false;
	smVec4 m_DiffuseColor;
	smuint8 m_DiffuseMapName[SM_TEXTURE_NAME_MAX_LENGTH];
};

class MaterialSystem SINGLETON(MaterialSystem)
{
public:
	SINGLETON_CONSTRUCTOR(MaterialSystem)

	smbool Init(const MaterialSystemConfig & config);
	void Shutdown();

	Material* Acquire(const smstring& name);
	void Release(const smstring& name);

	const Material& GetDefaultMaterial() { return m_DefaultMaterial; }
private:
	void DestroyMaterial(Material * material);
	smbool LoadMaterial(const MaterialConfig & config, Material * material);

	Material* AcquireFromConfig(const MaterialConfig & materialConfig);
	smbool LoadConfigurationFile(smcstring name, MaterialConfig & config);

	Material m_DefaultMaterial;

	MaterialSystemConfig m_Config;

	std::vector<Material> m_Materials;
	std::unordered_map<smstring, MaterialReference> m_MaterialLookup;
};

END_NAMESPACE