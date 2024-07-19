#pragma once
//#include <engine/memory/allocators/stackallocator.hpp>
#include <defines.hpp>

BEGIN_NAMESPACE

namespace Memory
{

	class SingleFrameAllocator
	{
    public:
        static SingleFrameAllocator& GetInstance() { static SingleFrameAllocator instance; return instance; }
        SingleFrameAllocator(SingleFrameAllocator& other) = delete;
        void operator=(const SingleFrameAllocator&) = delete;
    private:
        SingleFrameAllocator() {};
        ~SingleFrameAllocator() {}
    public:
        void* Allocate(muint64 size);
        void  Clear(void);

        static constexpr muint32 SINGLE_FRAME_ALLOCATOR_SIZE = 65536;
    private:
        //StackAllocator m_StackAllocator;
	};

}

END_NAMESPACE