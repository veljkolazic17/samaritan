#pragma once
#include <defines.hpp>
#include <utility>

/*
USE ONLY IN ENGINE CODE (Init, Shutdown)!
NEVER IN ENGINE LOOP (Run) !
*/

BEGIN_NAMESPACE

namespace Memory
{
	enum class MemoryTag
	{
		MEM_UNKNOW,
		MEM_ARRAY, 			// Used for all array like containers
		MEM_DICTIONARY,		// Used for all dictionary like containers
		MEM_STRING,
		MEM_TEXTURE,
		MEM_ENTITY,
		MEM_TASK,
		MEM_RENDERER,
		SIZE
	};

	// C++ style memory allocation
	template <typename T, typename... Args>
	T* mnew(Args&&... args)
	{
		return new (std::nothrow) T(std::forward<Args>(args)...);
	}

	template <typename T> void mdelete(T* object)
	{
		delete object;
	}
	// C style memory allocation
	void* mmaloc(muint64 size, mbool isAligned);
	void mmfree(void* memoryBlock, mbool isAligned);
	void* mmzero(void* memoryBlock, muint64 size);
	void* mmcpy(void* source, const void* destination, muint64 size);
	void* mmset(void* destination, muint32 value, muint64 size);

	void* Allocate(muint64 size, mbool isAligned, MemoryTag tag);
	void Free(void* memoryBlock, mbool isAligned, MemoryTag tag);
	void* Zero(void* memoryBlock, muint64 size);
	void* Copy(void* source, const void* destination, muint64 size);
	void* Set(void* destination, muint32 value, muint64 size); 


}

END_NAMESPACE
