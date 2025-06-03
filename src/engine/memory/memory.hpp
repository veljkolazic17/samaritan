#pragma once
#include <defines.hpp>
#include <utility>
#include <engine/memory/memorystats.hpp>

#define PAGE_SIZE	1024

#define GP_ALLOC_CONSTRAINT(tag) hardAssert(tag != samaritan::Memory::MemoryTag::MEM_PAGE, "Cannot be used for allocation vm page!");
#define VM_ALLOC_CONSTRAINT(tag) hardAssert(tag == samaritan::Memory::MemoryTag::MEM_PAGE, "Cannot be used for allocation that is not vm page!");

BEGIN_NAMESPACE

namespace Memory
{
	// GP C++ style memory allocation
	template <typename T, typename... Args>
	T* mnew(MemoryTag tag, Args&&... args)
	{
		GP_ALLOC_CONSTRAINT(tag);
#ifdef MEM_STATS_ENABLED
		MemoryStats::GetInstance().MemoryTagAllocate(sizeof(T), tag);
#endif
		return new (std::nothrow) T(std::forward<Args>(args)...);
	}

	template <typename T> void mdelete(T* object)
	{
		delete object;
	}
	// GP C style memory allocation
	void* mmaloc(smuint64 size, smbool isAligned);
	void mmfree(void* memoryBlock, smbool isAligned);
	void* mmsmZero(void* memoryBlock, smuint64 size);
	void* mmcpy(const void* source, void* destination, smuint64 size);
	void* mmset(void* destination, smuint32 value, smuint64 size);

	// Heap allocations ( do not use often )
	void* GPAllocate(smuint64 size, MemoryTag tag);
	void GPFree(void* memoryBlock, smuint64 size, MemoryTag tag);
	
	// Virtual memory page allocation ( allocates 4KiB )
	void* VMAllocate(smuint64 size, MemoryTag tag);
	void VMFree(void*memory, smuint64 size, MemoryTag tag);
}

END_NAMESPACE

#define smZero(memoryBlock, size)			samaritan::Memory::mmsmZero(memoryBlock, size)
#define smCopy(source, destination, size)	samaritan::Memory::mmcpy(source, destination, size)
#define smSet(destination, value, size)	samaritan::Memory::mmset(destination, value, size)

// Heap style general purpose allocator macros
#define gpAllocArray(size)				samaritan::Memory::GPAllocate(size, samaritan::Memory::MemoryTag::MEM_ARRAY);
#define gpAllocDictionary(size)			samaritan::Memory::GPAllocate(size, samaritan::Memory::MemoryTag::MEM_DICTIONARY);
#define gpAllocString(size)				samaritan::Memory::GPAllocate(size, samaritan::Memory::MemoryTag::MEM_STRING);
#define gpAllocTexture(size)			samaritan::Memory::GPAllocate(size, samaritan::Memory::MemoryTag::MEM_TEXTURE);
#define gpAllocEntity(size)				samaritan::Memory::GPAllocate(size, samaritan::Memory::MemoryTag::MEM_ENTITY);
#define gpAllocTask(size)				samaritan::Memory::GPAllocate(size, samaritan::Memory::MemoryTag::MEM_TASK);
#define gpAllocRenderer(size)			samaritan::Memory::GPAllocate(size, samaritan::Memory::MemoryTag::MEM_RENDERER);

#define gpFreeArray(memory, size)		samaritan::Memory::GPFree(memory, size, samaritan::Memory::MemoryTag::MEM_ARRAY);
#define gpFreeDictionary(memory, size)	samaritan::Memory::GPFree(memory, size, samaritan::Memory::MemoryTag::MEM_DICTIONARY);
#define gpFreeString(memory, size)		samaritan::Memory::GPFree(memory, size, samaritan::Memory::MemoryTag::MEM_STRING);
#define gpFreeTexture(memory, size)		samaritan::Memory::GPFree(memory, size, samaritan::Memory::MemoryTag::MEM_TEXTURE);
#define gpFreeEntity(memory, size)		samaritan::Memory::GPFree(memory, size, samaritan::Memory::MemoryTag::MEM_ENTITY);
#define gpFreeTask(memory, size)		samaritan::Memory::GPFree(memory, size, samaritan::Memory::MemoryTag::MEM_TASK);
#define gpFreeRenderer(memory, size)	samaritan::Memory::GPFree(memory, size, samaritan::Memory::MemoryTag::MEM_RENDERER);

// Page allocations macros 
#define vmAllocPage(size)				samaritan::Memory::VMAllocate(size, MemoryTag::MEM_PAGE);
#define vmFreePage(memory, size)		samaritan::Memory::VMAllocate(memory, size, MemoryTag::MEM_PAGE);
