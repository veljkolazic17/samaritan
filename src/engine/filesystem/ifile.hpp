#include <defines.hpp>

#include<ios>

typedef std::ios_base::openmode smfilemode;

BEGIN_NAMESPACE

class IFile
{
public:
	virtual void Open(const smint8* filepath, smfilemode filemode) = 0;
	virtual void Close() = 0;
	virtual void Read(void* buffer) = 0;
	virtual void Write(const void* buffer, smuint32 size) = 0;
	virtual smuint32 Length() = 0;
};

END_NAMESPACE