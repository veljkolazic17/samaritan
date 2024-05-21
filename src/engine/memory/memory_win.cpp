#ifdef TARGET_WIN
#include <engine/memory/memory.hpp>

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
}

END_NAMESPACE
#endif