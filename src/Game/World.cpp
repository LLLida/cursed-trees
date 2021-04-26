#include "World.hpp"
#include "Tree.hpp"

#include <functional>
#include <entt/entity/handle.hpp>

namespace game
{
	World::World(entt::registry& registry, unsigned int w, unsigned int h)
		: field(new entt::entity[w*h]), registry(registry), w(w), h(h)
	{
		registry.reserve<Cell>((w * h) >> 1); /* There will be a lot of cells */
		registry.reserve<Tree>((w * h) >> 2); /* and trees */
		for (unsigned int i = 0; i < w * h; i++)
			field[i] = registry.create();
	}

	World::~World() noexcept
	{
		if (field)
		{
			registry.clear<Tree>();
			for (unsigned int i = 0; i < w * h; i++)
				registry.destroy(field[i]);
			delete[] field;
		}
	}

	World::World(World&& world) noexcept
		: field(world.field), registry(world.registry), w(world.w), h(world.h)
	{
		world.field = nullptr;
	}

	World& World::operator=(World&& world) noexcept
	{
		this->~World();
		new(this) World(std::move(world));
		return *this;
	}

	entt::entity World::at(Vector2 pos) const
	{
		if (pos.x >= w || pos.y >= h)
		{
			throw std::out_of_range("pos is out of bounds");
		}
		return field[pos.y * w + pos.x];
	}

	void World::sun(int min, unsigned int levels)
	{
		for (unsigned int x = 0; x < w; x++)
		{
			unsigned int level = levels, height = h;
			entt::entity current;
			while (level && height)
			{
				height--;
				current = at({x, height});
				if (!registry.orphan(current))
				{
					auto& tree = registry.get<Tree>(registry.get<Cell>(current).parent);
					tree.energy += level * (height + min);
					level--;
				}
			}
		}
		for (auto&& [entity, cell] : registry.view<Cell>().each())
			registry.get<Tree>(cell.parent).energy -= 10;
	}

	void World::physics()
	{
		for (auto&& [entity, tree, falling] : registry.view<Tree, Falling>().each())
		{
			Vector2& pos = tree.aliveCells.front();
			if (pos.y == 0) /* if seed is at the bottom then plant it */
				[[unlikely]]
				Tree::plant(*this, entity);
			else
			{
				entt::entity below = at(pos.offset(Direction::DOWN));
				if (registry.any_of<Cell>(below)) /* destroy falling seed if there is
													 a Cell bellow */
					Tree::destroy(*this, entity);
				else /* move seed 1 tile down */
				{
					entt::entity seed = at(pos);
					registry.emplace<Cell>(below, std::move(registry.get<Cell>(seed)));
					registry.remove<Cell>(seed);
					pos.y--;
				}
			}
		}
	}

	void World::growTrees()
	{
		auto view = registry.view<Tree, Living>();
		for (auto&& [entity, tree, living] : view.each())
		{
			if (tree.energy <= 0) Tree::destroy(*this, entity);
			else if (living.age >= living.maxAge) Tree::kill(*this, entity);
			else Tree::grow(*this, entity);
		}
	}

	bool World::tick(int min, unsigned int levels)
	{
		physics();
		growTrees();
		sun(min, levels);
		return registry.size<Tree>() > 0;
	}
}

