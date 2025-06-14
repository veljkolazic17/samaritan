#include <engine/memory/containers/singleframeallocator.hpp>

BEGIN_NAMESPACE

namespace Memory
{
    void* SingleFrameAllocator::Allocate(smuint64 size)
    {
        /*static mbool hasFiredFirstAllocation = false;
        if (hasFiredFirstAllocation == false)
        {
            m_StackAllocator.Init(SingleFrameAllocator::SINGLE_FRAME_ALLOCATOR_SIZE);
            hasFiredFirstAllocation = true;
        }
        return m_StackAllocator.Allocate(size);*/
        return nullptr;
    }

    void SingleFrameAllocator::Clear(void)
    {
        //m_StackAllocator.Clear();
    }
}

END_NAMESPACE