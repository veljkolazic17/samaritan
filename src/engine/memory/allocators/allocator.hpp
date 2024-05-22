#pragma once
#include <defines.hpp>

BEGIN_NAMESPACE

namespace Memory
{
	class Allocator
	{
	public:
		virtual void Init(muint64 allocatorSize) = 0;
		virtual void* Allocate(muint64 stackSize) = 0;

		inline muint64 GetAllocatorSize(void) { return m_AllocatorSize; }
		inline void* GetMemoryChunk(void) { return m_MemoryChunk; }
	protected:
		void* m_MemoryChunk = nullptr;
		muint64 m_AllocatorSize = 0;
	};
}

END_NAMESPACE