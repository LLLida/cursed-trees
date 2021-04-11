#include "Tree.hpp"
#include "World.hpp"

#include <functional>
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>
#include <effolkronium/random.hpp>

namespace game
{
	using Random = effolkronium::random_static;

	Tree::Tree(int energy, const Genom& genom)
		: energy(energy), genom(genom)
	{
		
	}

	Tree& Tree::spawn(World& world, unsigned int x, int energy)
	{
		auto& reg = world.registry;
		entt::entity entity = reg.create();
		auto& tree = reg.emplace<Tree>(entity, energy);
		tree.aliveCells.push_back({x, 0});
		reg.emplace<Living>(entity, Living{ 
				.colorIndex = Random::get(1, 6), 
				.age = 0, 
				.maxAge = Random::get(minMaxAge, maxMaxAge)
			});
		entt::entity cell = world.at({x, 0});
		reg.remove_if_exists<Cell>(cell);
		reg.emplace<Cell>(cell, entity, byte{0u}, Cell::Type::ACTIVE);
		return tree;
	}

	void Tree::grow(World& world, entt::entity entity)
	{
		auto& reg = world.registry;
		auto& tree = reg.get<Tree>(entity);
		for (auto it = tree.aliveCells.begin(); it != tree.aliveCells.end();)
		{
			Vector2 pos = *it;
			entt::entity e = world.at(pos);
			Cell& cell = reg.get<Cell>(e);
			auto& currentGene = tree.genom.getGenes()[cell.activeGene];
			bool growed = false;
			visitDirections([&](Direction dir) -> void {
				Vector2 newPos;
				if (dir == Direction::LEFT && pos.x == 0) newPos = {world.w-1, pos.y};
				else if (dir == Direction::RIGHT && pos.x == world.w-1) newPos = {0, pos.y};
				else newPos = pos.offset(dir);
				byte nextGene = currentGene.proteins[static_cast<byte>(dir)].nextGene;
				if (newPos.x < world.w && newPos.y < world.h && nextGene < Genom::num_genes)
				{
					entt::entity near = world.at(newPos);
					if (!reg.any_of<Cell>(near) && 
						tree.genom.grows(world, cell, dir, pos))
					{
						reg.emplace<Cell>(near, cell).activeGene = nextGene;
						tree.aliveCells.push_front(newPos);
						growed = true;
					}
				}
			});
			if (growed)
			{
				tree.deadCells.push_back(pos);
				cell.type = Cell::Type::DEAD;
				it = tree.aliveCells.erase(it);
			}
			else ++it;
		}
		reg.get<Living>(entity).age++;
	}

	void Tree::kill(World& world, entt::entity entity)
	{
		auto& reg = world.registry;
		Tree& tree = reg.get<Tree>(entity);
		for (auto& pos : tree.deadCells)
			reg.remove<Cell>(world.at(pos));
		int averageEnergy = tree.energy / tree.aliveCells.size();
		for (auto& pos : tree.aliveCells)
		{
			entt::entity seed = reg.create();
			reg.emplace<Tree>(seed, averageEnergy, tree.genom.clone())
				.aliveCells.push_back(pos);
			reg.emplace<Falling>(seed);
			if (!reg.all_of<Cell>(world.at(pos)))
			{
				if (!reg.valid(world.at(pos)))
					throw std::runtime_error("Found invalid entity e: " 
											 + std::to_string(averageEnergy) 
											 + " pos: " + std::to_string(pos.x) + ", " + std::to_string(pos.y));
				if (reg.orphan(world.at(pos)))
					throw std::runtime_error("Found an entity with no cell component assigned e: " 
											 + std::to_string(averageEnergy) 
											 + " pos: " + std::to_string(pos.x) + ", " + std::to_string(pos.y)
											 + " num_cells: " + std::to_string(tree.aliveCells.size()));
				throw std::runtime_error("Found undefined entity");
			}
			auto& cell = reg.get<Cell>(world.at(pos));
			cell.parent = seed;
			cell.activeGene = 0;
		}
		reg.destroy(entity);
	}

	void Tree::destroy(World& world, entt::entity entity)
	{
		auto& reg = world.registry;
		auto& tree = reg.get<Tree>(entity);
		for (auto& pos : tree.aliveCells)
			reg.remove<Cell>(world.at(pos));
		for (auto& pos : tree.deadCells)
			reg.remove<Cell>(world.at(pos));
		reg.destroy(entity);
	}

	void Tree::plant(World& world, entt::entity entity)
	{
		auto& reg = world.registry;
		reg.remove<Falling>(entity);
		reg.emplace<Living>(entity, Living{ 
				.colorIndex = Random::get(1, 6), 
				.age = 0,
				.maxAge = Random::get(minMaxAge, maxMaxAge)
			});
	}

	int Tree::minMaxAge = 80;
	int Tree::maxMaxAge = 90;
}

