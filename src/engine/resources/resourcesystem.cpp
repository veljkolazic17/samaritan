#include <engine/resources/resourcesystem.hpp>

BEGIN_NAMESPACE

void ResourceSystem::Shutdown()
{
    // Clear all resources and control blocks
    m_Resources.clear();
    m_ControlBlocks.clear();
}

void ResourceSystem::SingletonInit()
{
    IMGUI_DISPLAY(smImguiCentral().RegisterImguiModule("Engine/Resource System", this);)
}

// ! This will be only called once reference count reaches zero for resource handle
void ResourceSystem::Release(ResourceControlBlock* controlBlock)
{
    if (controlBlock == nullptr) 
    {
        softAssert(false, "ResourceSystem: Attempted to release a null control block.");
        return;
    }

    const std::string& resourceName = controlBlock->m_Resource->m_Name;

    auto controlBlockIt = m_ControlBlocks.find(resourceName);
    auto resourceIt = m_Resources.find(resourceName);

    // controlBlockIt->second.get() == controlBlock check is crucial to prevent issues if a resource was force-unloaded and then a lingering handle tries to release.
    if (controlBlockIt != m_ControlBlocks.end() && resourceIt != m_Resources.end() && controlBlockIt->second.get() == controlBlock)
    {
        if (Resource* resource = controlBlock->m_Resource) 
        {
            resource->OnUnload();
        }

        //TODO : [RESOURCE] Not sure if this is neede since we have no other references for this resource
        controlBlock->m_Resource = nullptr;

        // Remove from maps. This will deallocate the unique_ptr and thus the Resource.
        m_ControlBlocks.erase(controlBlockIt);
        m_Resources.erase(resourceIt);
    }
    else
    {
        // This can happen if ForceUnload already removed it, or if there's a logic error.
        LogError(LogChannel::Resource, "ResourceSystem: Attempted to release control block for '%s' but it was not found or was already replaced in maps.", resourceName.c_str())
    }
}

#if IMGUI_DISPLAY_ENABLED
	void ResourceSystem::DrawImgui()
    {
        ImGui::Begin("Resource System");

        std::lock_guard<std::mutex> guard(m_Lock);

        ImGui::Text("Loaded Resources:");
        for (const auto& [name, controlBlock] : m_ControlBlocks)
        {
            if (const Resource* resource = controlBlock->m_Resource)
            {
                ImGui::BulletText("[%s] -> [Type: %s, State: %s, RefCount: %u]", 
                    name.c_str(), 
                    static_cast<const char*>(Resource::ResourceTypeToString(resource->m_Type)), 
                    static_cast<const char*>(Resource::ResourceStateToString(resource->m_State)), 
                    controlBlock->m_RefCount.load(std::memory_order_relaxed));
            }
            else
            {
                ImGui::BulletText("[%s] -> [Resource pointer is null]", name.c_str());
            }
        }

        ImGui::End();
    }
#endif


END_NAMESPACE