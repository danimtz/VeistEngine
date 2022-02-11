#pragma once

#include <atomic>
#include <bitset>

namespace Veist
{

namespace ecs
{

	constexpr uint32_t MAX_ENTITIES = 1000;
	constexpr uint32_t MAX_COMPONENTS = 32;

	using ComponentId = std::uint8_t;
	using EntityId = std::int32_t;
	using Signature = std::bitset<MAX_COMPONENTS>;


	inline bool isEntityIdValid(EntityId id)
	{
		//return (id >> 32) != EntityId(-1);
		return (id>=0);
	}

	uint32_t getNextComponentId();

	template<typename T>
	ComponentId getComponentId()
	{
		static ComponentId component_id = getNextComponentId();
		return component_id;
	}

	
}
}