#pragma once
#include <defines.hpp>
#include <objecttemplates/singleton.hpp>

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
		MEM_PAGE,
		SIZE
	};

	class MemoryStats SINGLETON(MemoryStats)
	{
	private:
		SINGLETON_CONSTRUCTOR(MemoryStats)
	public:
		void MemoryTagAllocate(smuint64 size, MemoryTag tag);
		void MemoryTagFree(smuint64 size, MemoryTag tag);
		void Clear();
		void MemoryStatsLog();
	private:
		// TODO : is 64 bits enough???
		smuint64 m_MemoryTagSizes[(smuint16)MemoryTag::SIZE];
	};
}

END_NAMESPACE