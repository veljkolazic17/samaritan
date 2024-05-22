#pragma once
#include <defines.hpp>
#include <engine/memory/allocators/allocator.hpp>

BEGIN_NAMESPACE

namespace Memory
{
	typedef muint64 marker;

	class StackAllocator : protected Allocator
	{
	public:
		void Init(muint64 allocatorSize);
		void* Allocate(muint64 stackSize);
		void Rollback(marker memMarker);
		inline marker GetMarker(void) { return m_CurrentChunkSize; }
		void Clear();

		~StackAllocator();
	private:
		muint64 m_StackPointer = 0;
		muint64 m_CurrentChunkSize = 0;
		void* m_StackTail = nullptr;
	};
}

END_NAMESPACE