#pragma once

namespace Veist
{
	namespace Utils
	{
		//boost
		template<typename T>
		inline constexpr void hash_combine(T& seed, T value)
		{
			seed ^= value + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		}
	}


}