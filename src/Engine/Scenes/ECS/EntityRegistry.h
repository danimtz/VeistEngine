#pragma once

#include "Engine/Scenes/ECS/ECSTypes.h"
#include "Engine/Scenes/ECS/ComponentPool.h"

namespace ecs{


uint32_t getNextComponentId();



class EntityRegistry
{
public:

	EntityRegistry();

	//Move constructor
	EntityRegistry(EntityRegistry&& other) 
		: m_entity_count{ std::move(other.m_entity_count) },
		m_free_entities{ std::move(other.m_free_entities) },
		m_entity_masks{ std::move(other.m_entity_masks) },
		m_component_pools{ std::move(other.m_component_pools)/*TODO: Test this*/ }{}
	
	//Move assignment operator 
	EntityRegistry &operator=(EntityRegistry&& other)
	{
		m_entity_count =  std::move(other.m_entity_count);
		m_free_entities = std::move(other.m_free_entities);
		m_entity_masks = std::move(other.m_entity_masks);
		m_component_pools = std::move(other.m_component_pools); //TODO: Test this
	}

	
	

	~EntityRegistry() = default;

	EntityId createEntity();
	void destroyEntity(EntityId entity);


	template<typename T, typename... Args>
	T& emplaceComponent(EntityId id, Args&&... args)//TODO recursive default constructor emplaceComponents <component1, component2, etc>
	{
		//Add and get component
		auto component = findOrCreateComponentPool<T>()->addComponent(id, std::forward<Args>(args)...);

		//Add component to entity mask
		EntityMask& entity_mask = getEntityMask(id);
		entity_mask.set( getComponentId<T>() ,true);

		return component;
	}

	template<typename T>
	void removeComponent(EntityId id)//TODO recursive removeComponents <component1, component2, etc>
	{
		
		findOrCreateComponentPool<T>()->removeComponent(id);
		EntityMask& entity_mask = getEntityMask(id);
		entity_mask.set(getComponentId<T>(), false);

	}

private:
	
	
	template<typename T>
	ComponentId getComponentId()
	{
		static ComponentId component_id = getNextComponentId();
		return component_id;
	}

	EntityMask& getEntityMask(EntityId id)
	{
		return m_entity_masks[id];
	}

	


	template<typename T>
	ComponentPool<T>* findOrCreateComponentPool()
	{
		ComponentId component_id = getComponentId<T>();
		if (component_id >= m_component_pools.size())
		{
			m_component_pools.push_back(std::make_shared<ComponentPool<T>>() );
			assert(component_id == (m_component_pools.size()-1));

			return static_cast<ComponentPool<T>*>(m_component_pools[component_id].get());
		}
		else
		{
			return static_cast<ComponentPool<T>*>(m_component_pools[component_id].get());
		}
	}



private:

	uint32_t m_entity_count;

	//queue of unused entity ids
	std::queue<EntityId> m_free_entities;

	//array of entity component masks. array index is the entity id
	std::array<EntityMask, MAX_ENTITIES> m_entity_masks;

	std::vector<std::shared_ptr<ComponentPoolBase>> m_component_pools;

	
};


}