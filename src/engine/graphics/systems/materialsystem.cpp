#include <engine/graphics/systems/materialsystem.hpp>

#include <engine/graphics/renderer/frontend/rendererfrontend.hpp>
#include <engine/graphics/systems/texturesystem.hpp>

#include <utils/logger/log.hpp>
#include <fstream>
#include <nlohmann/json.hpp>

BEGIN_NAMESPACE

mbool MaterialSystem::Init(const MaterialSystemConfig& config)
{
	if (config.m_MaxMaterialCount == 0)
	{
		softAssert(false, "Material System Config has max material count 0!");
		return false;
	}

	m_Config = config;

	Material temp;
	std::fill_n(std::back_inserter(m_Materials), config.m_MaxMaterialCount, temp);

	//Create default material
	std::strncpy(reinterpret_cast<char*>(m_DefaultMaterial.m_Name), SM_DEFAULT_MATERIAL_NAME, SM_MATERIAL_NAME_MAX_LENGTH);
	m_DefaultMaterial.m_ID = SM_INVALID_ID;
	m_DefaultMaterial.m_Generation = SM_INVALID_ID;
	m_DefaultMaterial.m_DiffuseColor = smVec4_one;
	m_DefaultMaterial.m_DiffuseMap.m_Type = TextureUsageType::TEXTURE_USAGE_MAP_DIFFUSE;
	m_DefaultMaterial.m_DiffuseMap.m_Texture = &smTextureSystem().GetDefaultTexture();
	
	if (!smRenderer().CreateMaterial(&m_DefaultMaterial))
	{
		softAssert(false, "Cannot create default material!");
		return false;
	}
	return true;
}

void MaterialSystem::Shutdown()
{
	Graphics::Renderer& renderer = smRenderer();
	renderer.DestroyMaterial(&m_DefaultMaterial);
	for (Material& material : m_Materials)
	{
		renderer.DestroyMaterial(&material);
	}
}

Material* MaterialSystem::Acquire(mcstring name)
{
	MaterialConfig materialConfig;
	if (LoadConfigurationFile(name, materialConfig))
	{
		return AcquireFromConfig(materialConfig);
	}
	return nullptr;
}

Material* MaterialSystem::AcquireFromConfig(const MaterialConfig& materialConfig)
{
	if (!std::strcmp(reinterpret_cast<const char*>(materialConfig.m_Name), SM_DEFAULT_MATERIAL_NAME))
	{
		return &m_DefaultMaterial;
	}

	MaterialReference& reference = m_MaterialLookup[reinterpret_cast<mcstring>(materialConfig.m_Name)];

	if (reference.m_RefCount == 0)
	{
		reference.m_ShouldAutoRelease = materialConfig.m_ShouldAutoRelease;
	}

	++reference.m_RefCount;
	if (reference.m_Handle == SM_INVALID_ID)
	{
		//Find free space in array
		//TODO : [TEXTURE] change this not to be vector
		Material* newMaterial = nullptr;
		for (muint64 counter = 0; counter < m_Materials.size(); ++counter)
		{
			Material& material = m_Materials[counter];
			if (material.m_ID == SM_INVALID_ID)
			{
				newMaterial = &material;
				reference.m_Handle = counter;
				break;
			}
		}

		if (newMaterial == nullptr)
		{
			softAssert(false, "Can't load more textures!");
			return &m_DefaultMaterial;
		}

		if (!LoadMaterial(materialConfig, newMaterial))
		{
			softAssert(false, "Failed to load texture %s", name);
			return &m_DefaultMaterial;
		}

		newMaterial->m_ID = reference.m_Handle;
		return newMaterial;
	}
	return nullptr;
}

void MaterialSystem::Release(mcstring name)
{
	if (!std::strcmp(name, SM_DEFAULT_MATERIAL_NAME))
	{
		softAssert(false, "Trying to release default material!");
	}

	if (m_MaterialLookup.contains(name))
	{
		MaterialReference& reference = m_MaterialLookup[name];
		--reference.m_RefCount;

		if (reference.m_RefCount == 0 && reference.m_ShouldAutoRelease)
		{
			Material* material = &m_Materials[reference.m_Handle];
			DestroyMaterial(material);
			reference.m_Handle = SM_INVALID_ID;
			reference.m_ShouldAutoRelease = false;
		}
		//TODO : [MATERIAL] Check when reference should be removed from entries
	}
	else
	{
		softAssert(false, "Material not loaded!");
	}
}

mbool MaterialSystem::LoadConfigurationFile(mcstring name, MaterialConfig& config)
{
	constexpr mcstring pathFormat = "../../../assets/materials/{}.json";
	std::string path = std::format(pathFormat, name);

	std::ifstream f(path);
	if (f.fail())
	{
		LogError(LogChannel::Graphics, "Cannot opet material file!");
		return false;
	}
	nlohmann::json data = nlohmann::json::parse(f);

	std::strncpy(reinterpret_cast<char*>(config.m_Name), data[1].template get<std::string>().data(), SM_MATERIAL_NAME_MAX_LENGTH);
	config.m_DiffuseColor = smVec4::StringToVec4(data[2]);
	std::strncpy(reinterpret_cast<char*>(config.m_DiffuseMapName), data[3].template get<std::string>().data(), SM_TEXTURE_NAME_MAX_LENGTH);

	return true;
}

void MaterialSystem::DestroyMaterial(Material* material)
{
	if (material == nullptr)
	{
		softAssert(false, "Trying to destroy invalid material!");
		return;
	}

	if (Texture* texture = material->m_DiffuseMap.m_Texture)
	{
		smTextureSystem().Release(reinterpret_cast<mcstring>(texture->m_Name));
	}

	smRenderer().DestroyMaterial(material);
	material->m_ID = SM_INVALID_ID;
	material->m_Generation = SM_INVALID_ID;
	material->m_InternalID = SM_INVALID_ID;
}

mbool MaterialSystem::LoadMaterial(const MaterialConfig& config, Material* material)
{
	smZero(material, sizeof(Material));
	std::strncpy(reinterpret_cast<char*>(material->m_Name), reinterpret_cast<const char*>(config.m_Name), SM_MATERIAL_NAME_MAX_LENGTH);

	material->m_DiffuseColor = config.m_DiffuseColor;
	if (std::strlen(reinterpret_cast<const char*>(config.m_DiffuseMapName)) > 0)
	{
		material->m_DiffuseMap.m_Type = TextureUsageType::TEXTURE_USAGE_MAP_DIFFUSE;
		constexpr mbool shouldAutoRelease = true;
		material->m_DiffuseMap.m_Texture = smTextureSystem().Acquire(reinterpret_cast<mcstring>(config.m_DiffuseMapName), shouldAutoRelease);
		if (material->m_DiffuseMap.m_Texture == nullptr)
		{
			LogError(LogChannel::Graphics, "Failed to acquire texture");
			material->m_DiffuseMap.m_Texture = &smTextureSystem().GetDefaultTexture();
		}
	}
	else
	{
		material->m_DiffuseMap.m_Type = TextureUsageType::TEXTURE_USAGE_UNKNOWN;
		material->m_DiffuseMap.m_Texture = nullptr;
	}

	if (!smRenderer().CreateMaterial(material))
	{
		softAssert(false, "Failed to acquire resources for material!");
		return false;
	}

	if (material->m_ID == SM_INVALID_ID)
	{
		material->m_Generation = 0;
	}
	else
	{
		++material->m_Generation;
	}

	return true;
}

END_NAMESPACE