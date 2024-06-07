#include <engine/containers/container.hpp>
#include <utils/asserts/assert.hpp>

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

        virtual void Print()
        {
            // TODO : Should I log this?
        }

        void InsertAt(T& element, muint64 position)
        {

        }


        T& RemoveAt(muint64 position)
        {
            
        }

        void Push(T& element);
        T& Pop();
    private:
        void Resize()
        {

        }

        muint64 m_Size = 0;
        muint64 m_Capacity = 0;
        muint64 m_AllocatedSize = 0;
        T* m_MemChunk = nullptr;
    };
}

END_NAMESPACE