#include <engine/memory/memorystats.hpp>
#include <utils/asserts/assert.hpp>
#include <utils/logger/log.hpp>
#include <engine/memory/memory.hpp>

BEGIN_NAMESPACE

namespace Memory
{
	void MemoryStats::MemoryTagAllocate(muint64 size, MemoryTag tag)
	{
		softAssert(tag < MemoryTag::SIZE, "Memory tag not defined!");
		softAssert(tag != MemoryTag::MEM_UNKNOW, "Using uknown memory tag!");
		m_MemoryTagSizes[(muint16)tag] += size;
	}

	void MemoryStats::MemoryTagFree(muint64 size, MemoryTag tag)
	{
		softAssert(tag < MemoryTag::SIZE, "Memory tag not defined!");
		m_MemoryTagSizes[(muint16)tag] += size;
	}

	void MemoryStats::Clear()
	{
		for (muint16 tag = 0; tag < (muint16)MemoryTag::SIZE; ++tag)
		{
			m_MemoryTagSizes[tag] = 0;
		}
	}

	void MemoryStats::MemoryStatsLog()
	{
		// TODO : Fucking stupid way of doing this ! :D
		constexpr muint8 sizeOfMemoryTagString = 8;
		muint8 memoryTagStrings[(muint16)MemoryTag::SIZE][(muint16)sizeOfMemoryTagString] =
		{
			"UNKNOW ",
			"ARRAY  ",
			"DICT   ",
			"STRING ",
			"TEXTURE",
			"ENTITY ",
			"TASK   ",
			"RENDER "
		};

		for (int tag = 0; tag < (muint16)MemoryTag::SIZE; ++tag)
		{
			LogInfo(LogChannel::Memory, "Memory Tag %s is using %d bytes of memory!", memoryTagStrings[tag], m_MemoryTagSizes[tag]);
		}
	}
}

END_NAMESPACE