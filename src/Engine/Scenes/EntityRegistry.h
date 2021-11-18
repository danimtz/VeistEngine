#pragma once

#include "Engine/Scenes/ComponentPool.h"

namespace ecs{

uint32_t getNextComponentId();



class EntityRegistry
{
public:

	EntityRegistry();
	~EntityRegistry() = default;

	EntityId createEntity();
	void destroyEntity(EntityId entity);


	template<typename T, typename... Args>
	T& emplaceComponent(EntityId id, Args&&... args)//TODO recursive default constructor emplaceComponents
	{
		return findOrCreateComponentPool<T>()->addComponent(id, std::forward<Args>(args)...);
	}

	template<typename T>
	void removeComponent(EntityId id)//TODO recursive removeComponents
	{
		return findOrCreateComponentPool<T>()->removeComponent(id);
	}

private:
	
	
	template<typename T>
	ComponentId getComponentId()
	{
		static ComponentId component_id = getNextComponentId();
		return component_id;
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



	
	uint32_t m_entity_count;
	std::queue<EntityId> m_free_entities;

	std::vector<std::shared_ptr<ComponentPoolBase>> m_component_pools;

	
};


}