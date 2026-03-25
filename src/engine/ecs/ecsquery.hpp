#pragma once
#include <engine/ecs/ecsworld.hpp>

#include <algorithm>
#include <functional>

BEGIN_NAMESPACE

namespace ECS
{
    template<typename... TComponents>
    class Query
    {
        using TComponentList = Typelist<TComponents...>;

    public:
        Query(ECS::World* ecsWorld)
        : m_World{ ecsWorld }
        , m_Included{ ecsWorld->GetComponentStoragePtr<TComponents>()... }
        , m_Excluded{}
        , m_CachedCount{0}
        , m_Base{nullptr}
        {
            hardAssert(TComponentList::size == m_Included.size(), "Size mismatch between type list and included component list");

            SortByIntersection();
            CacheCount();
        }

        smsize Size() const
        {
            return m_CachedCount;
        }

        template<typename... TExclude>
        Query& Without()
        {
            m_Excluded = { m_World->GetComponentStoragePtr<TExclude>()... };
            CacheCount();
            return *this;
        }

        void Foreach(std::function<void(EntityId, TComponents&...)> func)
        {
            if(m_Base == nullptr)
            {
                return;
            }

            SortByIntersection();
            constexpr auto indices = std::make_index_sequence<sizeof...(TComponents)>();

            smsize count = 0;
            for(EntityId entity : m_Base->Entities())
            {
                if(m_World->IsAlive(entity) && ContainsAllIncluded(entity) && MissesAllExcluded(entity))
                {
                    ++count;
                    call(func, std::tuple_cat(std::make_tuple(entity), MakeComponentsTuple(entity, indices)));
                }
            }
            m_CachedCount = count;
        }

    private:
        // Sets m_Base to the smallest included sparse set so Foreach iterates
        // the fewest entities possible. m_Included order is preserved so that
        // the type<->index mapping used by MakeComponentsTuple stays valid.
        void SortByIntersection()
        {
            if(m_Included.empty())
            {
                m_Base = nullptr;
                return;
            }

            m_Base = *std::min_element(m_Included.begin(), m_Included.end(),
                [](ISparseSet* a, ISparseSet* b)
                {
                    return a->Size() < b->Size();
                });
        }

        void CacheCount()
        {
            if(m_Base == nullptr)
            {
                m_CachedCount = 0;
                return;
            }

            smsize count = 0;
            for(EntityId entity : m_Base->Entities())
            {
                if(m_World->IsAlive(entity) && ContainsAllIncluded(entity) && MissesAllExcluded(entity))
                {
                    ++count;
                }
            }
            m_CachedCount = count;
        }

        smbool ContainsAllIncluded(EntityId entity)
        {
            return std::all_of(m_Included.begin(), m_Included.end(), [=](ISparseSet* set){
                return set->Contains(entity);
            });
        }

        smbool MissesAllExcluded(EntityId entity)
        {
            if(m_Excluded.empty()) return true;

            return std::none_of(m_Excluded.begin(), m_Excluded.end(), [=](ISparseSet* set){
                return set->Contains(entity);
            });
        }

        // m_Included retains the original TComponents... order, so m_Included[Index]
        // always corresponds to the Nth type in the template parameter pack.
        template<smsize Index>
        auto& GetComponentForIndex(EntityId entity)
        {
            using TComponentAtIndex = typename TComponentList::template get<Index>;
            return static_cast<SparseSet<TComponentAtIndex>*>(m_Included[Index])->Get(entity);
        }

        template <smsize... Indices>
        auto MakeComponentsTuple(EntityId entity, std::index_sequence<Indices...>)
        {
            return std::make_tuple(std::ref(GetComponentForIndex<Indices>(entity))...);
        }

    private:
        smarray<ISparseSet*, sizeof...(TComponents)> m_Included{};
        smvector<ISparseSet*> m_Excluded{};
        smsize m_CachedCount{ 0 };
        ISparseSet* m_Base{ nullptr };

        ECS::World* m_World = nullptr;
    };
}

END_NAMESPACE
