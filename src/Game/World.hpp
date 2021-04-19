#pragma once

#include "Tree.hpp"

#include <entt/entity/fwd.hpp>

namespace game
{
	class World
	{
	private:
		entt::entity* field;

	public:
		entt::registry& registry;
		const unsigned int w;
		const unsigned int h;

		World(entt::registry& registry, unsigned int w, unsigned int h);
		~World() noexcept;
		World(World&& world) noexcept;
		World& operator=(World&& world) noexcept;

		entt::entity at(Vector2 pos) const;

		void sun(unsigned int min, unsigned int levels);
		void physics();
		void growTrees();
		bool tick(unsigned int min, unsigned int levels);
	};
}
