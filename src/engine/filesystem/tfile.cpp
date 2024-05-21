#include <engine/filesystem/tfile.hpp>

#include <engine/memory/memory.hpp>

#include <sstream>
#include <utils/asserts/assert.hpp>

BEGIN_NAMESPACE

void TFile::Open(const smint8* filepath, smfilemode filemode)
{
	m_FileHandle.open(filepath, filemode);
}

void TFile::Close()
{
	m_FileHandle.close();
}

void TFile::Read(void* buffer)
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

	std::stringstream stringbuffer;
	stringbuffer << m_FileHandle.rdbuf();
	
	const std::string& tstring = stringbuffer.str();

	smCopy(tstring.c_str(), buffer, tstring.size());
}

void TFile::Write(const void* buffer, smuint32 size)
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

	m_FileHandle << reinterpret_cast<const char*>(buffer);
}

smuint32 TFile::Length()
{
	m_FileHandle.seekg(0, std::ios_base::end);
	auto length = m_FileHandle.tellg();
	m_FileHandle.seekg(0, std::ios_base::beg);
	return length;
}

END_NAMESPACE