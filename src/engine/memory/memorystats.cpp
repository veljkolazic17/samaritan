#include <engine/memory/memorystats.hpp>
#include <utils/asserts/assert.hpp>
#include <utils/logger/log.hpp>
#include <engine/memory/memory.hpp>

BEGIN_NAMESPACE

namespace Memory
{
	void MemoryStats::MemoryTagAllocate(smuint64 size, MemoryTag tag)
	{
		softAssert(tag < MemoryTag::SIZE, "Memory tag not defined!");
		softAssert(tag != MemoryTag::MEM_UNKNOW, "Using uknown memory tag!");

		m_MemoryTagSizes[(smuint16)tag] += size;
	}

	void MemoryStats::MemoryTagFree(smuint64 size, MemoryTag tag)
	{
		softAssert(tag < MemoryTag::SIZE, "Memory tag not defined!");
		m_MemoryTagSizes[(smuint16)tag] += size;
	}

	void MemoryStats::Clear()
	{
		for (smuint16 tag = 0; tag < (smuint16)MemoryTag::SIZE; ++tag)
		{
			m_MemoryTagSizes[tag] = 0;
		}
	}

	void MemoryStats::MemoryStatsLog()
	{
		constexpr smuint8 sizeOfMemoryTagString = 8;
		smuint8 memoryTagStrings[(smuint16)MemoryTag::SIZE][(smuint16)sizeOfMemoryTagString] =
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

		for (int tag = 0; tag < (smuint16)MemoryTag::SIZE; ++tag)
		{
			LogInfo(LogChannel::Memory, "Memory Tag %s is using %d bytes of memory!", memoryTagStrings[tag], m_MemoryTagSizes[tag]);
		}
	}
}

END_NAMESPACE