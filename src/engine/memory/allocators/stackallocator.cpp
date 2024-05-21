#include <engine/memory/allocators/stackallocator.hpp>
#include <engine/memory/memory.hpp>
#include <utils/asserts/assert.hpp>

BEGIN_NAMESPACE

namespace Memory
{
	void StackAllocator::Init(muint64 allocatorSize)
	{
		m_AllocatorSize = allocatorSize;
		m_MemoryChunk = mmaloc(allocatorSize, false);
	}

	void* StackAllocator::Allocate(muint64 stackSize)
	{
		if (m_StackPointer + stackSize >= m_AllocatorSize)
		{
			softAssert(false, "Cannot allocate memory!");
			return nullptr;
		}
		m_StackPointer += stackSize;
		m_CurrentChunkSize = stackSize;
		m_MemoryChunk = (void*)((muint64*)m_MemoryChunk + m_StackPointer - m_CurrentChunkSize);
		return m_MemoryChunk;
	}

	void StackAllocator::Clear()
	{
		m_StackPointer = 0;
		m_CurrentChunkSize = 0;
		m_MemoryChunk = (void*)((muint64*)m_MemoryChunk - m_CurrentChunkSize);
		mmfree(m_MemoryChunk, false);
	}

	void StackAllocator::Rollback(marker memMarker)
	{
		m_StackPointer -= m_CurrentChunkSize;
		m_CurrentChunkSize = memMarker;
		m_MemoryChunk  = (void*)((muint64*)m_MemoryChunk - memMarker);
	}

}

END_NAMESPACE