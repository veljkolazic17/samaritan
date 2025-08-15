#pragma once
#include <string>

/*
---------------TYPES----------------
*/
// Integer types
typedef unsigned long	            smuint64;
typedef long			            smint64;
typedef unsigned int                smuint32;
typedef int                         smint32;
typedef unsigned short              smuint16;
typedef short                       smint16;
typedef unsigned char               smuint8;
typedef char                        smint8;

typedef unsigned int	            smflags;

// Precision types
typedef float                       smfloat32;
typedef double                      smfloat64;

// Logical types
typedef bool                        smbool;

// String Types
typedef const char*                 smcstring;
typedef                 std::string smstring;

// Engine
typedef double			            Time;

// Complex types
struct Range
{
    smuint64 m_Offset = 0;
    smuint64 m_Size = 0;
};
