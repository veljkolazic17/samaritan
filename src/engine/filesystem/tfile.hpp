#pragma once
#include <defines.hpp>
#include <engine/filesystem/ifile.hpp>
#include <fstream>

BEGIN_NAMESPACE

class TFile : public IFile
{
public:
	void Open(const smint8* filepath, smfilemode filemode) override;
	void Close() override;
	void Read(void* buffer) override;
	void Write(const void* buffer, smuint32 size) override;
	smuint32 Length() override;
private:
	std::fstream m_FileHandle;
};

END_NAMESPACE