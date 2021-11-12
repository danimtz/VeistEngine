#pragma once



namespace ecs
{
constexpr uint32_t MAX_ENTITIES = 1000;

using ComponentId = std::uint8_t;
using EntityId = std::uint32_t;


class ComponentPoolBase
{
public:
	virtual ~ComponentPoolBase() = default;

	virtual void addComponent(EntityId entity)
	{

	}


};



template<typename T>
class ComponentPool final : public ComponentPoolBase
{
public:

	ComponentPool() = default;


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

		//m_component_array[new_index] = T(std::forward<Args>(args)...);
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