#pragma once
#include <defines.hpp>
#include <utility>

#define PAGE_SIZE	1024

BEGIN_NAMESPACE

namespace Memory
{
	enum class MemoryTag;
}

namespace Memory
{
	// GP C++ style memory allocation
	template <typename T, typename... Args>
	T* mnew(Args&&... args)
	{
		return new (std::nothrow) T(std::forward<Args>(args)...);
	}

	template <typename T> void mdelete(T* object)
	{
		delete object;
	}
	// GP C style memory allocation
	void* mmaloc(muint64 size, mbool isAligned);
	void mmfree(void* memoryBlock, mbool isAligned);
	void* mmzero(void* memoryBlock, muint64 size);
	void* mmcpy(const void* source, void* destination, muint64 size);
	void* mmset(void* destination, muint32 value, muint64 size);

	// Heap allocations ( do not use often )
	void* GPAllocate(muint64 size, MemoryTag tag);
	void GPFree(void* memoryBlock, muint64 size, MemoryTag tag);
	
	// Virtual memory page allocation ( allocates 4KiB )
	void* VMAllocate(muint64 size, MemoryTag tag);
	void VMFree(void*memory, muint64 size, MemoryTag tag);
}

END_NAMESPACE

#define Zero(memoryBlock, size)			samaritan::Memory::mmzero(memoryBlock, size)
#define Copy(source, destination, size)	samaritan::Memory::mmcpy(source, destination, size)
#define Set(destination, value, size)	samaritan::Memory::mmset(destination, value, size)

// Heap style general purpose allocator macros
#define gpAllocArray(size)				GPAllocate(size, MemoryTag::MEM_ARRAY);
#define gpAllocDictionary(size)			GPAllocate(size, MemoryTag::MEM_DICTIONARY);
#define gpAllocString(size)				GPAllocate(size, MemoryTag::MEM_STRING);
#define gpAllocTexture(size)			GPAllocate(size, MemoryTag::MEM_TEXTURE);
#define gpAllocEntity(size)				GPAllocate(size, MemoryTag::MEM_ENTITY);
#define gpAllocTask(size)				GPAllocate(size, MemoryTag::MEM_TASK);
#define gpAllocRenderer(size)			GPAllocate(size, MemoryTag::MEM_RENDERER);

#define gpFreeArray(memory, size)		GPFree(memory, size, MemoryTag::MEM_ARRAY);
#define gpFreeDictionary(memory, size)	GPFree(memory, size, MemoryTag::MEM_DICTIONARY);
#define gpFreeString(memory, size)		GPFree(memory, size, MemoryTag::MEM_STRING);
#define gpFreeTexture(memory, size)		GPFree(memory, size, MemoryTag::MEM_TEXTURE);
#define gpFreeEntity(memory, size)		GPFree(memory, size, MemoryTag::MEM_ENTITY);
#define gpFreeTask(memory, size)		GPFree(memory, size, MemoryTag::MEM_TASK);
#define gpFreeRenderer(memory, size)	GPFree(memory, size, MemoryTag::MEM_RENDERER);

// Page allocations macros 
#define vmAllocPage(size)				VMAllocate(size, MemoryTag::MEM_PAGE);
#define vmFreePage(memory, size)		VMAllocate(memory, size, MemoryTag::MEM_PAGE);

#define GP_ALLOC_CONSTRAINT(tag) hardAssert(tag != MemoryTag::MEM_PAGE, "Cannot be used for allocation vm page!");
#define VM_ALLOC_CONSTRAINT(tag) hardAssert(tag == MemoryTag::MEM_PAGE, "Cannot be used for allocation that is not vm page!");

