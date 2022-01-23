#pragma once

#include "Engine/Scenes/ECS/ECSTypes.h"

namespace ecs
{


class ComponentPoolBase
{
public:
	virtual ~ComponentPoolBase() = default;
	virtual void entityDestroyed(EntityId entity) = 0;

};



template<typename T>
class ComponentPool final : public ComponentPoolBase
{
public:

	ComponentPool() = default;


	//Adds a component to the pool. Variadic arguments for custom constructor of components
	//Returns reference to the component
	template<typename... Args>
	T& addComponent(EntityId entity, Args&&... args)
	{
		if (m_entity_to_idx_map.find(entity) != m_entity_to_idx_map.end())
		{
			CRITICAL_ERROR_LOG("Cannot add the same component to an entity more than once");
		}

		uint32_t new_index = m_component_count;
		m_entity_to_idx_map[entity] = new_index;
		m_idx_to_entity_map[new_index] = entity;

		if constexpr (sizeof...(Args))
		{
			m_component_array[new_index] = T(std::forward<Args>(args)...);
		}
		else
		{
			m_component_array[new_index] = T();
		}
		m_component_count++;
		return m_component_array[new_index];
	}

	//Adds a component to the pool. Variadic arguments for custom constructor of components
	void removeComponent(EntityId entity)
	{
		//TODO: Use entity version or double unordered map(like it is currently) to maintain sparse set
		
		if (m_entity_to_idx_map.find(entity) == m_entity_to_idx_map.end())
		{
			CRITICAL_ERROR_LOG("Component to be removed could not be found");
		}

		//Remove and move component in component array
		uint32_t removed_component_idx = m_entity_to_idx_map[entity];
		uint32_t last_component_idx = m_component_count-1;
		m_component_array[removed_component_idx] = std::move(m_component_array[last_component_idx]);

		//Update maps
		EntityId last_component_entity = m_idx_to_entity_map[last_component_idx];
		m_entity_to_idx_map[last_component_entity] = removed_component_idx;
		m_idx_to_entity_map[removed_component_idx] = last_component_entity;

		m_entity_to_idx_map.erase(entity);
		m_idx_to_entity_map.erase(last_component_idx);

		m_component_count--;
	}


	//Gets the component data of given entity
	T& getComponent(EntityId entity)
	{
		//TODO: change these if statements to asserts maybe?
		if (m_entity_to_idx_map.find(entity) == m_entity_to_idx_map.end())
		{
			CRITICAL_ERROR_LOG("Component to be removed could not be found");
		}

		return m_component_array[m_entity_to_idx_map[entity]];
	}



	void entityDestroyed(EntityId entity) override
	{
		if (m_entity_to_idx_map.find(entity) != m_entity_to_idx_map.end())
		{
			removeComponent(entity);
		}
	}

private:

	//Packed component array
	std::array<T, ecs::MAX_ENTITIES> m_component_array;

	//Number of valid entries in the packed array
	size_t m_component_count;

	//Sparse maps one from entity id to packed array index and one from packed array index to entity id
	std::unordered_map<EntityId, size_t> m_entity_to_idx_map;
	std::unordered_map<EntityId, size_t> m_idx_to_entity_map;

};

}