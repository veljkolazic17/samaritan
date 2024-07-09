#pragma once
#include <defines.hpp>

BEGIN_NAMESPACE

namespace Memory
{
	class Allocator
	{
	public:
		virtual void* Allocate(muint64 size) = 0;
		virtual void Deallocate(void* memory) = 0;
	};
}

END_NAMESPACE