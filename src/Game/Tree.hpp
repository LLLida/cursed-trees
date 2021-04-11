#pragma once

#include "Components.hpp"
#include "Genetic.hpp"

#include <list>

#include <lida/MemoryPool.hpp>

namespace game
{
	class World;

	class Tree
	{
	public:
		int energy;
		/* list with tree's active cells */
		std::list<Vector2, lida::MemoryPool<Vector2>> aliveCells;
        /* list with tree's dead cells */
		std::list<Vector2, lida::MemoryPool<Vector2>> deadCells;

		Genom genom;

		Tree(int energy, const Genom& genom = {});

		static Tree& spawn(World& world, unsigned int x, int energy = 300);
		static void grow(World& world, entt::entity tree);
		static void kill(World& world, entt::entity tree);
		static void destroy(World& world, entt::entity tree);
		static void plant(World& world, entt::entity tree);

		/* default value is 80 */
		static int minMaxAge;
		/* default value is 90 */
		static int maxMaxAge;
	};
}
