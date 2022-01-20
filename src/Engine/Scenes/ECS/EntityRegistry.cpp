
#include "EntityRegistry.h"
#include <atomic>

namespace ecs{


uint32_t getNextComponentId()
{
	static std::atomic<uint32_t> global_component_id_count = 0;
	return global_component_id_count++;

}



EntityRegistry::EntityRegistry() : m_entity_count(0)
{
	//fill entity id pool
	for (uint32_t i = 0; i < MAX_ENTITIES; i++)
	{
		m_free_entities.push(i);
	}

	
}



EntityId EntityRegistry::createEntity()
{
	if (m_entity_count >= MAX_ENTITIES)
	{
		CRITICAL_ERROR_LOG("Max entity count reached, cannot create more!")
	}
	
	EntityId id = m_free_entities.front();
	m_free_entities.pop();
	m_entity_count++;

	return id;
}

void EntityRegistry::destroyEntity(EntityId entity)
{
	if (entity > MAX_ENTITIES)
	{
		CRITICAL_ERROR_LOG("Entity to be deleted out of range")
	}

	//TODO additional destroying entity functionality herte

	m_free_entities.push(entity);
	m_entity_count--;

}

}