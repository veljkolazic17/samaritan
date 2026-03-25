#pragma once
#include <defines.hpp>
#include <engine/ecs/ecsworld.hpp>
#include <engine/ecs/ecsquery.hpp>
#include <engine/components/persistentid.hpp>
#include <engine/world/serialization/componentserializer.hpp>

#include <sqlite3.h>
#include <functional>
#include <unordered_map>
#include <vector>

BEGIN_NAMESPACE

class WorldSerializer
{
    using PersistentMap = std::unordered_map<smuint64, ECS::EntityId>;
    using LoadFn = std::function<void(sqlite3*, ECS::World&, const PersistentMap&)>;
    using SaveFn = std::function<void(sqlite3*, ECS::World&, smuint64&)>;

public:
    WorldSerializer() : m_Db(nullptr), m_NextId(1) {}

    void Init(sqlite3* db)
    {
        m_Db = db;
        ExecSQL("CREATE TABLE IF NOT EXISTS ENTITIES (ID INTEGER PRIMARY KEY);");
        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(m_Db, "SELECT COALESCE(MAX(ID), 0) FROM ENTITIES;", -1, &stmt, nullptr) == SQLITE_OK)
        {
            if (sqlite3_step(stmt) == SQLITE_ROW)
            {
                m_NextId = static_cast<smuint64>(sqlite3_column_int64(stmt, 0)) + 1;
            }
            sqlite3_finalize(stmt);
        }
    }

    template<typename T>
    void Register()
    {
        using S = ComponentSerializer<T>;

        m_SaveFns.push_back([](sqlite3* db, ECS::World& world, smuint64& nextId)
        {
            S::CreateTable(db);
            ECS::Query<T> query(&world);
            query.Foreach([&](ECS::EntityId runtimeId, T& comp)
            {
                if (!world.Has<PersistentId>(runtimeId))
                {
                    world.Add<PersistentId>(runtimeId, PersistentId(nextId++));
                }

                smuint64 persistentId = world.Get<PersistentId>(runtimeId).m_Id;

                sqlite3_stmt* estmt = nullptr;
                sqlite3_prepare_v2(db, "INSERT OR IGNORE INTO ENTITIES (ID) VALUES (?);", -1, &estmt, nullptr);
                sqlite3_bind_int64(estmt, 1, static_cast<sqlite3_int64>(persistentId));
                sqlite3_step(estmt);
                sqlite3_finalize(estmt);

                smstring sql = "INSERT OR REPLACE INTO " + S::TableName() + " (ID" + S::ColumnList() + ") VALUES (?" + S::Placeholders() + ");";
                sqlite3_stmt* stmt = nullptr;
                if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
                {
                    return;
                }

                sqlite3_bind_int64(stmt, 1, static_cast<sqlite3_int64>(persistentId));
                smuint32 col = 2;
                S::Bind(stmt, col, comp);
                sqlite3_step(stmt);
                sqlite3_finalize(stmt);
            });
        });

        m_LoadFns.push_back([](sqlite3* db, ECS::World& world, const PersistentMap& map)
        {
            for (auto& [persistentId, runtimeId] : map)
            {
                smstring sql = "SELECT * FROM " + S::TableName() + " WHERE ID = ?;";
                sqlite3_stmt* stmt = nullptr;
                if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
                {
                    continue;
                }

                sqlite3_bind_int64(stmt, 1, static_cast<sqlite3_int64>(persistentId));
                if (sqlite3_step(stmt) == SQLITE_ROW)
                {
                    T comp{};
                    smuint32 col = 1;
                    S::Read(stmt, col, comp);
                    world.Add<T>(runtimeId, std::move(comp));
                }
                sqlite3_finalize(stmt);
            }
        });
    }

    void SaveAll(ECS::World& world)
    {
        for (auto& saveFunctions : m_SaveFns)
        {
            saveFunctions(m_Db, world, m_NextId);
        }
    }

    void LoadAll(ECS::World& world)
    {
        PersistentMap map;

        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(m_Db, "SELECT ID FROM ENTITIES;", -1, &stmt, nullptr) != SQLITE_OK)
        {
            return;
        }

        while (sqlite3_step(stmt) == SQLITE_ROW)
        {
            smuint64 persistentId = static_cast<smuint64>(sqlite3_column_int64(stmt, 0));
            ECS::EntityId runtimeId = world.Spawn();
            world.Add<PersistentId>(runtimeId, PersistentId(persistentId));
            map[persistentId] = runtimeId;
        }
        sqlite3_finalize(stmt);

        for (auto& loadFunction : m_LoadFns)
        {
            loadFunction(m_Db, world, map);
        }
    }

private:
    void ExecSQL(const smstring& sql)
    {
        char* err = nullptr;
        sqlite3_exec(m_Db, sql.c_str(), nullptr, nullptr, &err);
        if (err) sqlite3_free(err);
    }

    sqlite3*  m_Db;
    smuint64  m_NextId;
    std::vector<SaveFn> m_SaveFns;
    std::vector<LoadFn> m_LoadFns;
};

END_NAMESPACE
