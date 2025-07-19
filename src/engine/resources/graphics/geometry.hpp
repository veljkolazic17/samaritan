#pragma once
#include <defines.hpp>

#define SM_DEFAULT_GEOMETRY_NAME    "DEFAULT"
#define SM_GEOMETRY_NAME_MAX_LENGTH 1024

BEGIN_NAMESPACE

struct Material;

struct Geometry
{
    smuint8 m_Name[SM_GEOMETRY_NAME_MAX_LENGTH];
    smuint32 m_Id = SM_INVALID_ID;
    smuint32 m_Handle = SM_INVALID_ID;
    smuint32 m_Generation = SM_INVALID_ID;
    Material* m_Material = nullptr;
};

END_NAMESPACE