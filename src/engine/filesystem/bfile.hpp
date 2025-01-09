#pragma once
#include <defines.hpp>
#include <engine/filesystem/ifile.hpp>
#include <fstream>

BEGIN_NAMESPACE

class BFile : public IFile
{
public:
	void Open(const mint8* filepath, smfilemode filemode) override;
	void Close() override;
	void Read(void* buffer) override;
	void Write(const void* buffer, muint32 size) override;
	muint32 Length() override;
private:
	std::fstream m_FileHandle;
};

END_NAMESPACE