#include <engine/memory/allocators/stackallocator.hpp>
#include <engine/memory/memory.hpp>
#include <utils/asserts/assert.hpp>

BEGIN_NAMESPACE

namespace Memory
{
	void StackAllocator::Init(muint64 allocatorSize)
	{
		if (m_MemoryChunk == nullptr)
		{
			m_AllocatorSize = allocatorSize;
			m_MemoryChunk = mmaloc(allocatorSize, false);
			m_StackTail = m_MemoryChunk;
		}
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
		m_StackTail = (void*)((muint64*)m_StackTail + m_StackPointer - m_CurrentChunkSize);
		return m_StackTail;
	}

	void StackAllocator::Clear()
	{
		// This means that allocator has been initialized
		if (m_AllocatorSize != 0)
		{
			m_StackPointer = 0;
			m_CurrentChunkSize = 0;
			m_StackTail = (void*)((muint64*)m_StackTail - m_CurrentChunkSize);
			mmzero(m_StackTail, m_AllocatorSize);
		}
	}

	void StackAllocator::Rollback(marker memMarker)
	{
		m_StackPointer -= m_CurrentChunkSize;
		m_CurrentChunkSize = memMarker;
		m_StackTail = (void*)((muint64*)m_StackTail - memMarker);
	}

	StackAllocator::~StackAllocator()
	{
		mmfree(m_MemoryChunk, false);
	}
}

END_NAMESPACE