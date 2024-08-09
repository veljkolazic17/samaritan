#pragma once
#include <defines.hpp>
#include <engine/memory/memorystats.hpp>

BEGIN_NAMESPACE

namespace Memory
{
	class Allocator
	{
	public:
		virtual void* Allocate(muint64 size) = 0;
		virtual void Deallocate(void* memory) = 0;

		virtual SM_INLINE MemoryTag GetMemoryTag() { return m_MemoryTag; }
		virtual SM_INLINE void SetMemoryTag(const MemoryTag& memoryTag) { m_MemoryTag = memoryTag; }

	protected:
		MemoryTag m_MemoryTag = MemoryTag::MEM_UNKNOW;
	};
}

END_NAMESPACE