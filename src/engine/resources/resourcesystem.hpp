#pragma once
#include <defines.hpp>

#include <engine/resources/resource.hpp>
#include <objecttemplates/singleton.hpp>
#include <utils/logger/log.hpp>

#include <mutex>
#include <unordered_map>

#define smResourceSystem() ::samaritan::ResourceSystem::GetInstance()

BEGIN_NAMESPACE

class ResourceSystem SINGLETON(ResourceSystem)
{
public:
    SINGLETON_CONSTRUCTOR(ResourceSystem)

    void Shutdown();

    template<typename T>
    ResourceHandle<T> Load(const std::string& name, smbool shouldAutoRelase = true)
    {

        static_assert(std::is_base_of<Resource, T>::value, "T must derive from Resource");
        std::lock_guard<std::mutex> guard(m_Lock);

        auto controlIt = m_ControlBlocks.find(name);
        if (controlIt != m_ControlBlocks.end())
        {
            return ResourceHandle<T>(controlIt->second.get());
        }

        //TODO : [RESOURCE][FUCKED] DO NOT USE ALLOCATION LIKE THIS! Find a way to allocate reosurces
        std::unique_ptr<T> newResource = std::make_unique<T>(name);
        T* rawResourcePtr = newResource.get();
        rawResourcePtr->OnLoad();
        rawResourcePtr->m_Name = std::move(name);

        m_Resources[name] = std::move(newResource);
        std::unique_ptr<ResourceControlBlock> newControlBlock = std::make_unique<ResourceControlBlock>(rawResourcePtr);
        m_ControlBlocks[name] = std::move(newControlBlock);

        ResourceHandle<T> handle = ResourceHandle<T>(m_ControlBlocks[name].get(), shouldAutoRelase);
        HACK(handle.m_Release = [this](ResourceControlBlock* controlBlock) {Release(controlBlock);};)
        return handle;
    }

    template<typename T>
    ResourceHandle<T> Load(T* resource, smbool shouldAutoRelase = true)
    {

        static_assert(std::is_base_of<Resource, T>::value, "T must derive from Resource");
        std::lock_guard<std::mutex> guard(m_Lock);

        auto controlIt = m_ControlBlocks.find(resource->m_Name);
        if (controlIt != m_ControlBlocks.end())
        {
            return ResourceHandle<T>(controlIt->second.get());
        }

        //m_Resources[resource->m_Name] = std::move(resource);
        std::unique_ptr<ResourceControlBlock> newControlBlock = std::make_unique<ResourceControlBlock>(resource);
        m_ControlBlocks[resource->m_Name] = std::move(newControlBlock);

        ResourceHandle<T> handle = ResourceHandle<T>(m_ControlBlocks[resource->m_Name].get(), shouldAutoRelase);
        HACK(handle.m_Release = [this](ResourceControlBlock* controlBlock) {Release(controlBlock);};)
        return handle;
    }

    // Any existing ResourceHandles to this resource are now dangling!
    // Their m_ControlBlock pointers will point to deallocated memory.
    // This is discouraged in a reference-counted system.
    // Use with caution!
    void ForceUnload(const std::string& name)
    {
        LogWarning(LogChannel::Resource, "Using ForceUnload! Are you sure that you should do this?");

        std::lock_guard<std::mutex> guard(m_Lock);

        auto controlIt = m_ControlBlocks.find(name);
        if (controlIt == m_ControlBlocks.end())
        {
            return;
        }

        ResourceControlBlock* controlBlock = controlIt->second.get();
        Resource* resourcePtr = controlBlock->m_Resource;
        if (resourcePtr != nullptr)
        {
            resourcePtr->OnUnload();
        }

        //In case there are still dangling references to this resource
        controlBlock->m_Resource = nullptr;

        m_ControlBlocks.erase(name);
        m_Resources.erase(name);
    }

    void Release(ResourceControlBlock* controlBlock);

private:
    //Not sure if there should be one
    std::mutex m_Lock;

    //TODO : [RESOURCE] Check if I should store resources like this
    std::unordered_map<std::string, std::unique_ptr<Resource>> m_Resources;
    //this is allocated in heap from resource system
    std::unordered_map<std::string, std::unique_ptr<ResourceControlBlock>> m_ControlBlocks;
};

END_NAMESPACE
