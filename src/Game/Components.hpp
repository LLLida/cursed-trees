#pragma once

#include <cstdint>

#include <entt/entity/fwd.hpp>

namespace game
{
	using byte = uint8_t;

	enum class Direction : byte
	{
		LEFT, DOWN, UP, RIGHT
	};

	/**
	 * @brief Call @a function on every direction.
	 * @param function an callable object which takes game::Direction as argument.
	 */
	inline void visitDirections(auto&& function)
	{
		function(Direction::LEFT);
		function(Direction::DOWN);
		function(Direction::UP);
		function(Direction::RIGHT);
	}

	struct Vector2
	{ 
		unsigned int x, y; 

		inline Vector2 operator+(const Vector2& rhs) const
		{
			return {x + rhs.x, y + rhs.y};
		}

		inline Vector2 offset(Direction dir) const
		{
			switch(dir)
			{
				case Direction::LEFT: return {x-1, y};
				case Direction::DOWN: return {x, y-1};
				case Direction::UP: return {x, y+1};
				case Direction::RIGHT: return {x+1, y};
				default: return *this;
			}
		}
	};

	struct Falling
	{
		int velocity;
	};

	struct Living
	{
		int colorIndex = 0;
		int age = 0;
		int maxAge = 0;
	};
	
	struct Cell
	{
		enum class Type : byte
		{
			ACTIVE, DEAD
		};

		entt::entity parent;
		byte activeGene;
		Type type;
	};
}
