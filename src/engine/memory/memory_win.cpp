#ifdef TARGET_WIN
#include <Windows.h>
#include <engine/memory/memory.hpp>
#include <engine/memory/memorystats.hpp>

BEGIN_NAMESPACE

namespace Memory
{
	void* mmaloc(muint64 size, mbool isAligned)
	{
		return malloc(size);
	}

	void mmfree(void* memoryBlock, mbool isAligned)
	{
		free(memoryBlock);
	}

	void* mmzero(void* memoryBlock, muint64 size)
	{
		return memset(memoryBlock, 0, size);
	}

	void* mmcpy(const void* source, void* destination, muint64 size)
	{
		return memcpy(destination, source, size);
	}

	void* mmset(void* destination, muint32 value, muint64 size)
	{
		return memset(destination, value, size);
	}

	void* GPAllocate(muint64 size, MemoryTag tag)
	{
		GP_ALLOC_CONSTRAINT(tag);
#ifdef MEM_STATS_ENABLED
		MemoryStats::GetInstance().MemoryTagAllocate(size, tag);
#endif
		return mmaloc(size, false);
	}

	void GPFree(void* memoryBlock, muint64 size, MemoryTag tag)
	{
		GP_ALLOC_CONSTRAINT(tag);
#ifdef MEM_STATS_ENABLED
		MemoryStats::GetInstance().MemoryTagFree(size, tag);
#endif
		mmfree(memoryBlock, false);
	}

	void* VMAllocate(muint64 size, MemoryTag tag)
	{
		VM_ALLOC_CONSTRAINT(tag);
#ifdef MEM_STATS_ENABLED
		MemoryStats::GetInstance().MemoryTagAllocate(size, tag);
#endif
		// TODO : should there be commit ? or should commit be done later ?
		return VirtualAlloc(0, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	}

	void* VMFree(void* memory, muint64 size, MemoryTag tag)
	{
		VM_ALLOC_CONSTRAINT(tag);
#ifdef MEM_STATS_ENABLED
		MemoryStats::GetInstance().MemoryTagFree(size, tag);
#endif
		VirtualFree(memory, 0, MEM_RELEASE);
	}
}

END_NAMESPACE
#endif