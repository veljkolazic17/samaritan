#pragma once
#include <engine/ecs/ecstypes.hpp>

#include <utils/asserts/assert.hpp>

#define ECS_CHECK_VALID_ENTITY(entity) \
hardAssert(entity != EMPTY_ENTITY, "Invalid entity!");\
hardAssert(entity >= 0 && entity < m_CurrentEntity, "Invalid Entity!");

#define ECS_CHECK_IS_IN_USE_ENTITY(entity) \
hardAssert(IsAlive(entity), "Invalid entity!");

#define TYPENAME(t) typeid(t).name()

BEGIN_NAMESPACE

namespace ECS
{

class World
{
    using ComponentMask = smbitset<MAX_COMPONENTS>;

public:
    World() : m_EntityMasks {}, m_ComponentStorage {}, m_CurrentEntity { 0 }
    {}

    smbool IsAlive(EntityId id)
    {
        return m_EntityMasks.GetPtr(id) != nullptr;
    }

    ComponentMask& GetEntityMask(EntityId id)
    {
        return m_EntityMasks.Get(id);
    }

    ComponentMask* GetEntityMaskPtr(EntityId id)
    {
        return m_EntityMasks.GetPtr(id);
    }

    smsize EntityCount()
    {
        return m_CurrentEntity;
    }

    smsize FreeEntityCount()
    {
        return m_EntityFreelist.size();
    }

    const smvector<EntityId>& GetEntityIds() const
    {
        return m_EntityMasks.Entities();
    }

    template<typename T>
    void RegisterComponent()
    {
        smsize componentCount = m_ComponentStorage.size();
        hardAssert(componentCount <= MAX_COMPONENTS, "Maximum number of components registered!");

        smsize index = GetComponentIndex<T>();
        if(index >= componentCount)
        {
            m_ComponentStorage.resize(index + 1);
        }

        softAssert(!m_ComponentStorage[index], "Component is already registered");

        m_ComponentStorage[index] = std::make_unique<SparseSet<T>>();
    }

    template<typename TComponent>
    TComponent& Add(EntityId entity, const TComponent&& val = {})
    {
        ECS_CHECK_VALID_ENTITY(entity);
        ECS_CHECK_IS_IN_USE_ENTITY(entity);

        SparseSet<TComponent>& set = GetTypedComponentStorageRef<TComponent>();

        hardAssert(set.GetPtr(entity) == nullptr, "Adding Component to entity that already has it!");

        ComponentMask& mask = GetEntityMask(entity);
        SetComponentMaskBit<TComponent>(mask, true);
        return *set.Set(entity, std::move(val));
    }

    template<typename TComponent>
    smbool Has(EntityId entity)
    {
        if(!IsAlive(entity))
        {
            return false;
        }

        ComponentMask& mask = GetEntityMask(entity);
        return GetComponentMaskBit<TComponent>(mask);
    }

    void RemoveAll(EntityId entity)
    {
        ECS_CHECK_VALID_ENTITY(entity);

        ComponentMask& mask = GetEntityMask(entity);
        for(smsize i = 0; i < m_ComponentStorage.size(); ++i)
        {
            SetComponentMaskBit(mask, i, 0);
            auto storage = m_ComponentStorage[i].get();
            if(storage != nullptr)
            {
                storage->Delete(entity);
            }
        }
    }

    template<typename TComponent>
    void Remove(EntityId entity)
    {
        ECS_CHECK_VALID_ENTITY(entity);
        ECS_CHECK_IS_IN_USE_ENTITY(entity);

        SparseSet<TComponent>& set = GetTypedComponentStorageRef<TComponent>();
        if(!set.Contains(entity))
        {
            return;
        }

        ComponentMask& mask = GetEntityMask(entity);
        SetComponentMaskBit<TComponent>(mask, 0);

        set.Delete(entity);
    }

    template<typename TComponent>
    TComponent& Get(EntityId entity)
    {
        ECS_CHECK_VALID_ENTITY(entity);
        ECS_CHECK_IS_IN_USE_ENTITY(entity);

        SparseSet<TComponent>& set = GetTypedComponentStorageRef<TComponent>();
        TComponent* comp = set.GetPtr(entity);

        hardAssert(comp != nullptr, "Entity is nullptr");
        return *comp;
    }
    
#ifdef SM_TOOL
    void ForEachComponent(EntityId entity, std::function<void(const smstring& componentName, ISparseSet& storage)> visitor)
    {
        ECS_CHECK_VALID_ENTITY(entity);
        ECS_CHECK_IS_IN_USE_ENTITY(entity);

        ComponentMask& mask = GetEntityMask(entity);
        for (smsize i = 0; i < m_ComponentStorage.size(); ++i)
        {
            if (mask[i] && m_ComponentStorage[i] && m_ComponentStorage[i]->Contains(entity))
            {
                visitor(ms_ComponentDebugLabels[i], *m_ComponentStorage[i]);
            }
        }
    }
#endif

    EntityId Spawn()
    {
        EntityId entity = EMPTY_ENTITY;

        if(m_EntityFreelist.size() == 0)
        {
            hardAssert(m_CurrentEntity < MAX_ENTITIES, "Cannot spawn more entities");
            entity = m_CurrentEntity++;
        }
        else
        {
            entity = m_EntityFreelist.back();
            m_EntityFreelist.pop_back();
        }

        ComponentMask* entityMask = GetEntityMaskPtr(entity);
        if(entityMask != nullptr)
        {
            ResetComponentMask(*entityMask);
        }

        hardAssert(entity != EMPTY_ENTITY, "Entity cannot be EMPTY_ENTITY");
        m_EntityMasks.Set(entity, {});

        return entity;
    }

    static smsize& GetStaticComponentCounter()
    {
        static smsize s_ComponentCounter{ 0 };
        return s_ComponentCounter;
    }

    static smsize PopNextComponentIndex(const char* debugName)
    {
        smsize& s_ComponentCounter = GetStaticComponentCounter();
        smstring debugNameString = smstring(debugName);

        ms_ComponentDebugLabels.push_back(std::move(debugNameString));
        return s_ComponentCounter++;
    }

    template<typename T>
    static smsize GetComponentIndex()
    {
        static smsize s_Index{PopNextComponentIndex(TYPENAME(T))};
        return s_Index;
    }

    template<typename T>
    smsize GetOrInsertComponentIndex()
    {
        const smsize index = GetComponentIndex<T>();
        if(index >= m_ComponentStorage.size() || !m_ComponentStorage[index])
        {
            RegisterComponent<T>();
        }

        hardAssert(index >= 0 && index < m_ComponentStorage.size(), "Index of ouf bounds");
        return index;
    }

    template<typename T>
    ISparseSet* GetComponentStoragePtr()
    {
        const smsize index = GetOrInsertComponentIndex<T>();
        return m_ComponentStorage[index].get();
    }

    template<typename T>
    SparseSet<T>& GetTypedComponentStorageRef()
    {
        return *static_cast<SparseSet<T>*>(GetComponentStoragePtr<T>());
    }

    template<typename T>
    void SetComponentMaskBit(ComponentMask& mask, smbool val)
    {
        mask[GetComponentIndex<T>()] = val;
    }

    void SetComponentMaskBit(ComponentMask& mask, smsize t, smbool val)
    {
        mask[t] = val;
    }
    
    template<typename T>
    ComponentMask::reference GetComponentMaskBit(ComponentMask& mask)
    {
        return mask[GetComponentIndex<T>()];
    }

    template<typename... TComponents>
    ComponentMask GetComponentMask()
    {
        ComponentMask mask;
        (SetComponentMaskBit<TComponents>(mask, true), ...);
        return mask;
    }

    void ResetComponentMask(ComponentMask& mask)
    {
        mask.reset();
    }

#ifdef DEBUG
    template<typename TComponent>
    smstring& GetComponentDebugLabel() const
    {
        return GetComponentDeugLabel(GetComponentIndex<TComponent>());
    }

    smstring& GetComponentDebugLabel(smsize component) const
    {
        return ms_ComponentDebugLabels[component];
    }
#endif

private:
    SparseSet<ComponentMask> m_EntityMasks;
    smvector<smowned<ISparseSet>> m_ComponentStorage;
    smvector<EntityId> m_EntityFreelist;
    EntityId m_CurrentEntity;

    static smvector<smstring> ms_ComponentDebugLabels;
};

}

END_NAMESPACE