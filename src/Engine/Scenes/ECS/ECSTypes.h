#pragma once

namespace ecs
{

	constexpr uint32_t MAX_ENTITIES = 1000;
	constexpr uint32_t MAX_COMPONENTS = 32;

	using ComponentId = std::uint8_t;
	using EntityId = std::uint32_t;
	using EntityMask = std::bitset<MAX_COMPONENTS>;


}