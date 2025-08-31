#pragma once
#include <engine/resources/resourcesystem.hpp>
#include <utils/asserts/assert.hpp>

BEGIN_NAMESPACE

template<typename T>
ResourceHandle<T>::ResourceHandle<T>(const ResourceHandle<T>& other) : m_ControlBlock(other.m_ControlBlock)
{
    if (m_ControlBlock != nullptr)
    {
        m_ControlBlock->Increment();
    }
}

template<typename T>
ResourceHandle<T>::ResourceHandle<T>(ResourceHandle<T>&& other) noexcept : m_ControlBlock(other.m_ControlBlock)
{
    other.m_ControlBlock = nullptr;
}

template<typename T>
ResourceHandle<T>::ResourceHandle<T>(T* resource, smbool shouldAutoRelease) : m_ControlBlock(nullptr), m_ShouldAutoRelease(shouldAutoRelease)
{
    /*auto& controlBlocks = smResourceSystem().m_ControlBlocks;
    auto it = controlBlocks.find(resource->m_Name);
    if (it != controlBlocks.end()) 
    {
        m_ControlBlock = it->second.get();
        m_ControlBlock->Increment();
    } 
    else
    {
        hardAssert(false, "ResourceHandle: Resource not found in ResourceSystem. Use ResourceSystem::Load to create a handle for a resource.");
    }*/
}


template<typename T>
T* ResourceHandle<T>::operator->() const
{
    return m_ControlBlock ? static_cast<T*>(m_ControlBlock->m_Resource) : nullptr;
}

template<typename T>
T& ResourceHandle<T>::operator*() const
{
    return *static_cast<T*>(m_ControlBlock->m_Resource);
}

template<typename T>
ResourceHandle<T>::operator bool() const
{
    return IsValid();
}

template<typename T>
T* ResourceHandle<T>::GetResource() const
{
    return m_ControlBlock ? static_cast<T*>(m_ControlBlock->m_Resource) : nullptr;
}

template<typename T>
ResourceHandle<T>& ResourceHandle<T>::operator=(const ResourceHandle<T>& other)
{
    if (this != &other) 
    {
        if (m_ControlBlock != nullptr)
        {
            m_ControlBlock->Decrement();
            if (m_ShouldAutoRelease && m_ControlBlock->m_RefCount <= 0)
            {
                HACK(m_Release(m_ControlBlock);)
            }
        }

        m_ControlBlock = other.m_ControlBlock;

        if (m_ControlBlock)
        {
            m_ControlBlock->Increment();
        }
    }
    return *this;
}

template<typename T>
ResourceHandle<T>& ResourceHandle<T>::operator=(ResourceHandle<T>&& other) noexcept
{
    if (this != &other) 
    {
        if (m_ControlBlock)
        {
            m_ControlBlock->Decrement();
            if (m_ShouldAutoRelease && m_ControlBlock->m_RefCount <= 0)
            {
                HACK(m_Release(m_ControlBlock);)
            }
        }

        m_ControlBlock = other.m_ControlBlock;
        other.m_ControlBlock = nullptr;
    }
    return *this;
}

template<typename T>
void ResourceHandle<T>::Reset()
{
    if (m_ControlBlock != nullptr)
    {
        m_ControlBlock->Decrement();
        if (m_ShouldAutoRelease && m_ControlBlock->m_RefCount <= 0)
        {
            HACK(m_Release(m_ControlBlock);)
        }
        m_ControlBlock = nullptr;
    }
}

template<typename T>
smbool ResourceHandle<T>::IsValid() const
{
    return m_ControlBlock != nullptr && m_ControlBlock->m_Resource != nullptr;
}

template<typename T>
smbool ResourceHandle<T>::IsValid()
{
    return m_ControlBlock != nullptr && m_ControlBlock->m_Resource != nullptr;
}

template<typename T>
ResourceHandle<T>::~ResourceHandle()
{
    if (m_ControlBlock != nullptr)
    {
        m_ControlBlock->Decrement();
        if (m_ShouldAutoRelease && m_ControlBlock->m_RefCount <= 0)
        {
            HACK(m_Release(m_ControlBlock);)
        }
    }
}

template<typename T>
ResourceHandle<T>& ResourceHandle<T>::InvalidReference()
{
    static ResourceHandle<T> invalidHandle;
    return invalidHandle;
}

END_NAMESPACE