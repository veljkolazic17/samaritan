//Implementation is based on lecture notes by [Linkedin: gavrilov-miroslav]
#pragma once
#include <defines.hpp>

#include <utils/asserts/assert.hpp>

#include <array>
#include <bitset>
#include <memory>

BEGIN_NAMESPACE

namespace ECS
{
    
using smsize = size_t;

template<smsize N>
using smbitset = std::bitset<N>;

template<typename T, smsize N>
using smarray = std::array<T, N>;

template<typename K, typename V>
using smmap = std::unordered_map<K, V>;

template<typename T>
using smset = std::unordered_set<T>;

template<typename...Ts>
using smtuple = std::tuple<Ts...>;

template <typename T>
using smvector = std::vector<T>;

template<smsize...Ts>
using smseq = std::index_sequence<Ts...>;

template<typename T>
using smlimits = std::numeric_limits<T>;

template<typename T>
using smowned = std::unique_ptr<T>;

template<typename T>
using smshared = std::shared_ptr<T>;

using EntityId = smsize;

static const EntityId EMPTY_ENTITY = (smlimits<EntityId>::max)();

static constexpr const smsize MAX_COMPONENTS = 1024;

static constexpr const smsize MAX_ENTITIES = (smsize)EMPTY_ENTITY;

template<typename Function, typename Tuple, size_t... I>
auto call(Function f, Tuple t, std::index_sequence<I...>)
{
    return f(std::get<I>(t)...);
}

template<typename Function, typename Tuple>
auto call(Function f, Tuple t)
{
    static constexpr auto size = std::tuple_size<Tuple>::value;
    return call(f, t, std::make_index_sequence<size>{});
}

template<typename... Ts>
struct Typelist
{
    using TTuple = smtuple<Ts...>;
    static constexpr smsize size = sizeof...(Ts);

    template<smsize At>
    using get = std::tuple_element_t<At, TTuple>;
};

struct ISparseSet
{
    virtual ~ISparseSet() = default;

    virtual smbool Contains(EntityId entity) const = 0;
    virtual smbool IsEmpty() const = 0;

    virtual smsize Size() const = 0;
    virtual const smvector<EntityId>& Entities() const = 0;

    virtual void Delete(EntityId entity) = 0;
    virtual void DeleteAll() = 0;
};

#define SplitPageIndexEntry(entityId) \
const smsize pageIndex = entityId / MAX_PAGE_COUNT; \
const smsize pageEntry = entityId % MAX_PAGE_COUNT;

template<typename T>
struct SparseSet : public ISparseSet
{
    SparseSet()
    {
        m_Dense.reserve(MAX_COMPONENTS);
        m_Data.reserve(MAX_COMPONENTS);
    }

    SparseSet(const SparseSet<T>& other) = default;
    SparseSet(SparseSet<T>&& other) = default;

    SparseSet<T>& operator=(const SparseSet<T>& other) = default;
    SparseSet<T>& operator=(SparseSet<T>&& other) = default;

private:
    static constexpr const smsize MAX_PAGE_COUNT = 1024;
    static constexpr const smsize TOMBSTONE = (smlimits<smsize>::max)();
    using PagedSparseArray = smarray<smsize, MAX_PAGE_COUNT>;

    smvector<PagedSparseArray> m_SparsePages;
    smvector<EntityId> m_Dense;
    smvector<T> m_Data;
private:
    void SetDenseIndex(EntityId id, smsize index)
    {
        SplitPageIndexEntry(id);

        if (pageIndex >= m_SparsePages.size())
        {
            m_SparsePages.resize(pageIndex + 1);
            m_SparsePages[pageIndex].fill(TOMBSTONE);
        }

        auto& page = m_SparsePages[pageIndex];
        page[pageEntry] = index;
    }

    smsize GetDenseIndex(EntityId id) const
    {
        SplitPageIndexEntry(id);

        if(pageIndex < m_SparsePages.size())
        {
            auto& page = m_SparsePages[pageIndex];
            return page[pageEntry];
        }

        return TOMBSTONE;
    }

public:
    SM_INLINE T* Set(EntityId id, T member)
    {
        const smsize index = GetDenseIndex(id);
        if(index != TOMBSTONE)
        {
            m_Dense[index] = id;
            m_Data[index] = member;

            return &m_Data[index];
        }

        SetDenseIndex(id, m_Data.size());

        m_Dense.push_back(id);
        m_Data.push_back(member);
        return &m_Data.back();
    }

    SM_INLINE T& Get(EntityId id)
    {
        auto index = GetDenseIndex(id);
        hardAssert(index != TOMBSTONE, "Invalid Entity");
        return m_Data[index];
    }

    SM_INLINE const T* GetPtr(EntityId id) const
    {
        auto index = GetDenseIndex(id);
        if(index != TOMBSTONE)
        {
            return &m_Data[index];
        }
        return nullptr;
    }

    SM_INLINE T* GetPtr(EntityId id)
    {
        auto index = GetDenseIndex(id);
        if(index != TOMBSTONE)
        {
            return &m_Data[index];
        }
        return nullptr;
    }

    SM_INLINE smbool Contains(EntityId id) const { return GetDenseIndex(id) != TOMBSTONE; }
    SM_INLINE smbool IsEmpty() const { return m_Data.empty(); }
    SM_INLINE smsize Size() const { return m_Data.size(); }
    SM_INLINE const smvector<EntityId>& Entities() const { return m_Dense; }
    SM_INLINE smvector<T>& Data() const { return m_Data; }

    void Delete(EntityId id)
    {
        auto deletedIndex = GetDenseIndex(id);
        if(m_Data.empty() || deletedIndex == TOMBSTONE)
        {
            return;
        }

        SetDenseIndex(m_Dense.back(), deletedIndex);
        SetDenseIndex(id, TOMBSTONE);

        std::swap(m_Data.back(), m_Data[deletedIndex]);
        std::swap(m_Dense.back(), m_Dense[deletedIndex]);

        m_Data.pop_back();
        m_Dense.pop_back();
    }

    void DeleteAll()
    {
        m_Dense.clear();
        m_Data.clear();
        m_SparsePages.clear();
    }
};
}

END_NAMESPACE