#pragma once

#include "Tree.hpp"

#include <array>

#include <entt/entity/fwd.hpp>

namespace game
{
	class World
	{
	private:
		entt::entity* const field;

	public:
		entt::registry& registry;
		const unsigned int w;
		const unsigned int h;

		World(entt::registry& registry, unsigned int w, unsigned int h);
		~World();

		entt::entity at(Vector2 pos) const;

		void sun(unsigned int min, unsigned int levels);
		void physics();
		void growTrees();
		bool tick(unsigned int min, unsigned int levels);
	};
}
