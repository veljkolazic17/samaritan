#pragma once
#include <defines.hpp>
#include <memory>
#include <functional>

BEGIN_NAMESPACE

class ResourceSystem;

enum class ResourceState
{
    Unloaded = 0,
    Loading,
    Loaded,
    Error
};

enum class ResourceType
{
    Undefined = 0,
    Texture,
    Material,
    Geometry,
    Animation,
    Audio,
    Script,
    Model,
    Font,
    Shader,
    COUNT
};

struct Resource
{
    Resource(){ m_ResourceId = Resource::GenerateResourceId(); };
    Resource(const std::string& name) : m_Name(name), m_Type(ResourceType::Undefined), m_State(ResourceState::Unloaded) { m_ResourceId = Resource::GenerateResourceId(); }
    Resource(ResourceType type, const std::string& name, const std::string& filePath) : m_Type(type), m_Name(name), m_FilePath(filePath) { m_ResourceId = Resource::GenerateResourceId(); }

    virtual void OnUnload() = 0;
    virtual void OnLoad() = 0;

    ResourceType m_Type = ResourceType::Undefined;
    ResourceState m_State = ResourceState::Unloaded;
    std::string m_Name;
    std::string m_FilePath;
    smuint64 m_ResourceId = SM_INVALID_ID;


    // TODO : [FUCKED] Not good can run out of ids
    static smuint64 GenerateResourceId()
    {
        static std::atomic<smuint64> resourceId = SM_INVALID_ID;
        resourceId.fetch_add(1, std::memory_order_relaxed);
        return resourceId;
    }
};

struct ResourceControlBlock 
{
    ResourceControlBlock(Resource* ressource) : m_Resource(ressource), m_RefCount(1) {}

    void Increment() 
    {
        m_RefCount.fetch_add(1, std::memory_order_relaxed);
    }

    void Decrement() 
    {
        m_RefCount.fetch_sub(1, std::memory_order_relaxed);
    }

    Resource* m_Resource = nullptr;
    std::atomic<unsigned int> m_RefCount = 0;
};

template <typename T>
class ResourceHandle 
{
    static_assert(std::is_base_of<Resource, T>::value, "ResourceHandle<T>: T must derive from Resource");

public:
    ResourceHandle() : m_ControlBlock(nullptr), m_ShouldAutoRelease(false) {}
    ResourceHandle(ResourceControlBlock* controlBlock, smbool shouldAutoRelease = true) : m_ControlBlock(controlBlock), m_ShouldAutoRelease(shouldAutoRelease) {}
    ResourceHandle(const ResourceHandle& other);
    ResourceHandle(ResourceHandle&& other) noexcept;
    explicit ResourceHandle(T* resource, smbool shouldAutoRelease = true);
    ResourceHandle& operator=(const ResourceHandle& other);
    ResourceHandle& operator=(ResourceHandle&& other) noexcept;
    ~ResourceHandle();
    T* operator->() const;
    T& operator*() const;
    operator bool() const;
    T* GetResource() const;

    smbool IsValid() const;
    smbool IsValid();

    static ResourceHandle& InvalidReference();

    void Reset();

private:
    ResourceControlBlock* m_ControlBlock;
    smbool m_ShouldAutoRelease = false;

    HACK(std::function<void(ResourceControlBlock*)> m_Release;)

    friend class ResourceSystem;
};


END_NAMESPACE

#include <engine/resources/resource.hxx>