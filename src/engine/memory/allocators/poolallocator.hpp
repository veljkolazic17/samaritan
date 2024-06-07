#pragma once
#include <defines.hpp>
#include <engine/memory/allocators/allocator.hpp>
#include <engine/memory/memory.hpp>

BEGIN_NAMESPACE

namespace Memory
{
	class PoolAllocator : protected Allocator
	{
	private:
		struct Chunk
		{
			Chunk* m_NextChunk;
		};

	public:
		void Init(muint64 numberOfChunksPerBlock)
		{
			/*m_NumberOfChunkPerBlock = numberOfChunksPerBlock;
			m_AllocatorSize = numberOfElements * sizeof(T);*/
		}

		void* Allocate(muint64 size)
		{
			if (m_MemoryChunk == nullptr) 
			{
				m_MemoryChunk = (void*)AllocateBlock(size);
			}

			Chunk* freeChunk = (Chunk*)m_MemoryChunk;

			(Chunk*)m_MemoryChunk = ((Chunk*)m_MemoryChunk)->m_NextChunk;

			return freeChunk;
		}

		void Deallocate(void* chunk, muint32 size)
		{
			((Chunk*)chunk)->m_NextChunk = (Chunk*)m_MemoryChunk;

			m_MemoryChunk = chunk;
		}

	private:
		Chunk* AllocateBlock(muint32 chunkSize)
		{
			muint64 blockSize = chunkSize * m_NumberOfChunkPerBlock;
			Chunk* blockHead = reinterpret_cast<Chunk*>(mmaloc(blockSize, false));

			Chunk* chunk = blockHead;

			// Link all of the chunks in newly created block
			for (int i = 0; i < m_NumberOfChunkPerBlock - 1; ++i)
			{
				chunk->m_NextChunk = reinterpret_cast<Chunk*>(reinterpret_cast<char*>(chunk) + chunkSize);
				chunk = chunk->m_NextChunk;
			}
			chunk->m_NextChunk = nullptr;

			return blockHead;
		}

		muint64 m_NumberOfChunkPerBlock = 0;
	};
}

END_NAMESPACE