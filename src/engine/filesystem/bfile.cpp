#include <engine/filesystem/bfile.hpp>

#include <engine/memory/memory.hpp>

#include <sstream>
#include <utils/asserts/assert.hpp>

BEGIN_NAMESPACE

void BFile::Open(const mint8* filepath, smfilemode filemode)
{
	m_FileHandle.open(filepath, filemode | std::ios::binary);
}

void BFile::Close()
{
	m_FileHandle.close();
}

void BFile::Read(void* buffer)
{
	if (!m_FileHandle.is_open())
	{
		softAssert(false, "File is not open!");
		return;
	}

	if (buffer == nullptr)
	{
		return;
	}

	m_FileHandle.read(reinterpret_cast<char*>(buffer), Length());
}

void BFile::Write(const void* buffer, muint32 size)
{
	if (!m_FileHandle.is_open())
	{
		softAssert(false, "File is not open!");
		return;
	}

	if (buffer == nullptr)
	{
		return;
	}

	m_FileHandle.write(reinterpret_cast<const char*>(buffer), size);
}

muint32 BFile::Length()
{
	m_FileHandle.seekg(0, std::ios_base::end);
	auto length = m_FileHandle.tellg();
	m_FileHandle.seekg(0, std::ios_base::beg);
	return length;
}

END_NAMESPACE