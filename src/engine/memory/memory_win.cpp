#ifdef TARGET_WIN
#include <Windows.h>
#include <engine/memory/memory.hpp>
#include <engine/memory/memorystats.hpp>

BEGIN_NAMESPACE

namespace Memory
{
	void* mmaloc(smuint64 size, smbool isAligned)
	{
		return malloc(size);
	}

	void mmfree(void* memoryBlock, smbool isAligned)
	{
		free(memoryBlock);
	}

	void* mmsmZero(void* memoryBlock, smuint64 size)
	{
		return memset(memoryBlock, 0, size);
	}

	void* mmcpy(const void* source, void* destination, smuint64 size)
	{
		return memcpy(destination, source, size);
	}

	void* mmset(void* destination, smuint32 value, smuint64 size)
	{
		return memset(destination, value, size);
	}

	void* GPAllocate(smuint64 size, MemoryTag tag)
	{
		GP_ALLOC_CONSTRAINT(tag);
#ifdef MEM_STATS_ENABLED
		MemoryStats::GetInstance().MemoryTagAllocate(size, tag);
#endif
		return mmaloc(size, false);
	}

	void GPFree(void* memoryBlock, smuint64 size, MemoryTag tag)
	{
		GP_ALLOC_CONSTRAINT(tag);
#ifdef MEM_STATS_ENABLED
		MemoryStats::GetInstance().MemoryTagFree(size, tag);
#endif
		mmfree(memoryBlock, false);
	}

	void* VMAllocate(smuint64 size, MemoryTag tag)
	{
		VM_ALLOC_CONSTRAINT(tag);
#ifdef MEM_STATS_ENABLED
		MemoryStats::GetInstance().MemoryTagAllocate(size, tag);
#endif
		// TODO : should there be commit ? or should commit be done later ?
		return VirtualAlloc(0, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	}

	void VMFree(void* memory, smuint64 size, MemoryTag tag)
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