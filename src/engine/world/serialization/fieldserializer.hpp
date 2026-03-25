#pragma once
#include <defines.hpp>

#include <math/vector.hpp>

#include <sqlite3.h>
#include <sstream>

BEGIN_NAMESPACE

template<typename T>
struct FieldSerializer;

template<>
struct FieldSerializer<smfloat32>
{
    static void Columns(std::ostringstream& sql, const smstring& name) { sql << ", " << name << " NUMERIC"; }
    static void Bind(sqlite3_stmt* stmt, smuint32& col, smfloat32 val)  { sqlite3_bind_double(stmt, col++, val); }
    static void Read(sqlite3_stmt* stmt, smuint32& col, smfloat32& val) { val = static_cast<smfloat32>(sqlite3_column_double(stmt, col++)); }
    static int  Count() { return 1; }
};

template<>
struct FieldSerializer<smfloat64>
{
    static void Columns(std::ostringstream& sql, const smstring& name) { sql << ", " << name << " NUMERIC"; }
    static void Bind(sqlite3_stmt* stmt, smuint32& col, smfloat64 val)  { sqlite3_bind_double(stmt, col++, val); }
    static void Read(sqlite3_stmt* stmt, smuint32& col, smfloat64& val) { val = sqlite3_column_double(stmt, col++); }
    static int  Count() { return 1; }
};

template<>
struct FieldSerializer<smint32>
{
    static void Columns(std::ostringstream& sql, const smstring& name) { sql << ", " << name << " INTEGER"; }
    static void Bind(sqlite3_stmt* stmt, smuint32& col, smint32 val)  { sqlite3_bind_int(stmt, col++, val); }
    static void Read(sqlite3_stmt* stmt, smuint32& col, smint32& val) { val = sqlite3_column_int(stmt, col++); }
    static int  Count() { return 1; }
};

template<>
struct FieldSerializer<smuint32>
{
    static void Columns(std::ostringstream& sql, const smstring& name) { sql << ", " << name << " INTEGER"; }
    static void Bind(sqlite3_stmt* stmt, smuint32& col, smuint32 val)  { sqlite3_bind_int64(stmt, col++, val); }
    static void Read(sqlite3_stmt* stmt, smuint32& col, smuint32& val) { val = static_cast<smuint32>(sqlite3_column_int64(stmt, col++)); }
    static int  Count() { return 1; }
};

template<>
struct FieldSerializer<smuint64>
{
    static void Columns(std::ostringstream& sql, const smstring& name) { sql << ", " << name << " INTEGER"; }
    static void Bind(sqlite3_stmt* stmt, smuint32& col, smuint64 val)  { sqlite3_bind_int64(stmt, col++, static_cast<sqlite3_int64>(val)); }
    static void Read(sqlite3_stmt* stmt, smuint32& col, smuint64& val) { val = static_cast<smuint64>(sqlite3_column_int64(stmt, col++)); }
    static int  Count() { return 1; }
};

template<>
struct FieldSerializer<smbool>
{
    static void Columns(std::ostringstream& sql, const smstring& name) { sql << ", " << name << " INTEGER"; }
    static void Bind(sqlite3_stmt* stmt, smuint32& col, smbool val)  { sqlite3_bind_int(stmt, col++, val ? 1 : 0); }
    static void Read(sqlite3_stmt* stmt, smuint32& col, smbool& val) { val = sqlite3_column_int(stmt, col++) != 0; }
    static int  Count() { return 1; }
};

template<>
struct FieldSerializer<smstring>
{
    static void Columns(std::ostringstream& sql, const smstring& name) { sql << ", " << name << " TEXT"; }
    static void Bind(sqlite3_stmt* stmt, smuint32& col, const smstring& val) { sqlite3_bind_text(stmt, col++, val.c_str(), -1, SQLITE_TRANSIENT); }
    static void Read(sqlite3_stmt* stmt, smuint32& col, smstring& val)       { val = reinterpret_cast<const char*>(sqlite3_column_text(stmt, col++)); }
    static int  Count() { return 1; }
};

template<>
struct FieldSerializer<smVec2>
{
    static void Columns(std::ostringstream& sql, const smstring& name)
    {
        sql << ", " << name << "_x NUMERIC";
        sql << ", " << name << "_y NUMERIC";
    }
    static void Bind(sqlite3_stmt* stmt, smuint32& col, const smVec2& v)
    {
        sqlite3_bind_double(stmt, col++, v.m_X);
        sqlite3_bind_double(stmt, col++, v.m_Y);
    }
    static void Read(sqlite3_stmt* stmt, smuint32& col, smVec2& v)
    {
        v.m_X = static_cast<smfloat32>(sqlite3_column_double(stmt, col++));
        v.m_Y = static_cast<smfloat32>(sqlite3_column_double(stmt, col++));
    }
    static int Count() { return 2; }
};

template<>
struct FieldSerializer<smVec3>
{
    static void Columns(std::ostringstream& sql, const smstring& name)
    {
        sql << ", " << name << "_x NUMERIC";
        sql << ", " << name << "_y NUMERIC";
        sql << ", " << name << "_z NUMERIC";
    }
    static void Bind(sqlite3_stmt* stmt, smuint32& col, const smVec3& v)
    {
        sqlite3_bind_double(stmt, col++, v.m_X);
        sqlite3_bind_double(stmt, col++, v.m_Y);
        sqlite3_bind_double(stmt, col++, v.m_Z);
    }
    static void Read(sqlite3_stmt* stmt, smuint32& col, smVec3& v)
    {
        v.m_X = static_cast<smfloat32>(sqlite3_column_double(stmt, col++));
        v.m_Y = static_cast<smfloat32>(sqlite3_column_double(stmt, col++));
        v.m_Z = static_cast<smfloat32>(sqlite3_column_double(stmt, col++));
    }
    static int Count() { return 3; }
};

template<>
struct FieldSerializer<smVec4>
{
    static void Columns(std::ostringstream& sql, const smstring& name)
    {
        sql << ", " << name << "_x NUMERIC";
        sql << ", " << name << "_y NUMERIC";
        sql << ", " << name << "_z NUMERIC";
        sql << ", " << name << "_w NUMERIC";
    }
    static void Bind(sqlite3_stmt* stmt, smuint32& col, const smVec4& v)
    {
        sqlite3_bind_double(stmt, col++, v.m_X);
        sqlite3_bind_double(stmt, col++, v.m_Y);
        sqlite3_bind_double(stmt, col++, v.m_Z);
        sqlite3_bind_double(stmt, col++, v.m_W);
    }
    static void Read(sqlite3_stmt* stmt, smuint32& col, smVec4& v)
    {
        v.m_X = static_cast<smfloat32>(sqlite3_column_double(stmt, col++));
        v.m_Y = static_cast<smfloat32>(sqlite3_column_double(stmt, col++));
        v.m_Z = static_cast<smfloat32>(sqlite3_column_double(stmt, col++));
        v.m_W = static_cast<smfloat32>(sqlite3_column_double(stmt, col++));
    }
    static int Count() { return 4; }
};


END_NAMESPACE
