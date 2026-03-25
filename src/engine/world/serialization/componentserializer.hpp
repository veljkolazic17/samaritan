#pragma once
#include <defines.hpp>
#include <engine/world/serialization/fieldserializer.hpp>

#include <refl.hpp>
#include <sqlite3.h>
#include <sstream>

BEGIN_NAMESPACE

template<typename T>
struct ComponentSerializer
{
    static smstring TableName()
    {
        smstring name = smstring(refl::reflect<T>().name.str());
        //Do this shit because db names can't support :: in name
        for (char& c : name)
        {
            if (c == ':') c = '_';
        }
        return name;
    }

    static smstring ColumnList()
    {
        std::ostringstream ss;
        refl::util::for_each(refl::reflect<T>().members, [&](auto member)
        {
            using F = typename decltype(member)::value_type;
            FieldSerializer<F>::Columns(ss, member.name.c_str());
        });
        return ss.str();
    }

    static smstring Placeholders()
    {
        smuint32 total = 0;
        refl::util::for_each(refl::reflect<T>().members, [&](auto member)
        {
            using F = typename decltype(member)::value_type;
            total += FieldSerializer<F>::Count();
        });

        smstring s;
        for (smuint32 i = 0; i < total; ++i)
        {
            s += ", ?";
        }
        return s;
    }

    static void CreateTable(sqlite3* db)
    {
        std::ostringstream sql;
        sql << "CREATE TABLE IF NOT EXISTS " << TableName() << " (ID INTEGER PRIMARY KEY";
        refl::util::for_each(refl::reflect<T>().members, [&](auto member)
        {
            using F = typename decltype(member)::value_type;
            FieldSerializer<F>::Columns(sql, member.name.c_str());
        });
        sql << ");";
        char* err = nullptr;
        sqlite3_exec(db, sql.str().c_str(), nullptr, nullptr, &err);
        if (err)
        {
            sqlite3_free(err);
        }
    }

    static void Bind(sqlite3_stmt* stmt, smuint32& col, const T& val)
    {
        refl::util::for_each(refl::reflect<T>().members, [&](auto member)
        {
            using F = typename decltype(member)::value_type;
            FieldSerializer<F>::Bind(stmt, col, member(val));
        });
    }

    static void Read(sqlite3_stmt* stmt, smuint32& col, T& val)
    {
        refl::util::for_each(refl::reflect<T>().members, [&](auto member)
        {
            using F = typename decltype(member)::value_type;
            FieldSerializer<F>::Read(stmt, col, member(val));
        });
    }
};

END_NAMESPACE

#define SERIALIZABLE_COMPONENT(ComponentType, ...) \
    REFL_AUTO(type(ComponentType), ##__VA_ARGS__)
