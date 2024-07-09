#pragma once
#include <engine/containers/container.hpp>
#include <utils/asserts/assert.hpp>
#include <engine/memory/memorystats.hpp>

BEGIN_NAMESPACE

namespace Containers
{
    template<typename T>
    class DynamicArray : public Container<T>
    {
    public:
        DynamicArray<T>(muint64 size)
        {
            m_Capacity = size;
            m_MemChunk = Memory::Allocate(size * sizeof(T), Memory::MemortyTag::Array);
            Clear();
        }

        virtual muint64 GetSize()
        {
            return m_Size;
        }

        virtual void Clear()
        {
            mmzero(m_MemChunk, m_Size * sizeof(T));
            m_Size = 0;
        }

        void InsertAt(T& element, muint64 position)
        {

        }

        T& RemoveAt(muint64 position)
        {
            
        }

        void Push(T& element)
        {
            InsertAt(element, 0);
        }

        T& Pop()
        {
            return RemoveAt(0);
        }

        T* GetAt(muint64 position)
        {
            hardAssert(position < m_Capacity, "Accessing out of bounds!");
            return m_MemChunk[position];
        }

    private:
        void Resize()
        {

        }

        constexpr muint64 strideSize = 16 * sizeof(T);

        muint64 m_Size = 0;
        muint64 m_Capacity = 0;
        muint64 m_AllocatedSize = 0;
        T* m_MemChunk = nullptr;
    };
}

END_NAMESPACE