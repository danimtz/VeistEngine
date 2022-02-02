#include "pch.h"

#include "EntityRegistry.h"


namespace Veist
{

namespace ecs{


uint32_t getNextComponentId() //TODO move to ECSTypes.cpp?
{
	static std::atomic<uint32_t> global_component_id_count = 0;
	return global_component_id_count++;

}


EntityRegistry::EntityRegistry() : m_entity_count(0)
{

}



EntityId EntityRegistry::createEntity()
{
	if (m_entity_count >= MAX_ENTITIES)
	{
		CRITICAL_ERROR_LOG("Max entity count reached, cannot create more!")
	}


	if (!m_free_entities.empty())
	{
		EntityId id = m_free_entities.front();
		m_free_entities.pop();
		m_entities[id] = id;
		m_entity_count++;
		return id;
	}
	else
	{
		m_entities.push_back(m_entities.size());
		EntityId id = m_entities.back();
		m_entity_count++;

		return id;
	}

	
}

void EntityRegistry::destroyEntity(EntityId entity)
{
	if (entity > MAX_ENTITIES)
	{
		CRITICAL_ERROR_LOG("Entity to be deleted out of range")
	}

	//Remove entity from entities and add to free list
	m_entities[entity] = -1;
	m_free_entities.push(entity);
	m_entity_count--;


	//Remove components associated from that entity
	for (auto& component_pool : m_component_pools)
	{
		component_pool->entityDestroyed(entity);
	}

	//Reset entity signature
	m_entity_signatures[entity].reset();

}

}
}