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
}

END_NAMESPACE
