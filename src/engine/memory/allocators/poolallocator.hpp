#pragma once
#include <defines.hpp>
#include <engine/memory/allocators/allocator.hpp>

BEGIN_NAMESPACE

namespace Memory
{
	template <class T>
	class PoolAllocator : protected Allocator
	{
	public:
		void Init(muint64 allocatorSize);
		void* Allocate(muint64 stackSize);
		void Clear();
	};
}

END_NAMESPACE