#include "Genetic.hpp"
#include "World.hpp"

#include <functional>
#include <entt/entity/handle.hpp>
#include <effolkronium/random.hpp>

namespace game
{
	using Random = effolkronium::random_static;

	bool Gene::grows(const World& world, entt::entity entity, Direction dir, const Vector2& pos) const
	{
		auto& reg = world.registry;
		auto& tree = reg.get<Tree>(entity);
		auto& living = reg.get<Living>(entity);
		auto& protein = proteins[static_cast<byte>(dir)];
		switch(protein.predicate)
		{
			case Gene::Predicate::ENERGY_LESS:
				return tree.energy <= protein.parameter * 50;
			case Gene::Predicate::ENERGY_GREATER:
				return tree.energy >= protein.parameter * 50;
			case Gene::Predicate::HEIGHT_LESS:
				return pos.y <= protein.parameter; 
			case Gene::Predicate::HEIGHT_GREATER:
				return pos.y >= protein.parameter; 
			case Gene::Predicate::AGE_LESS:
				return living.age <= protein.parameter; 
			case Gene::Predicate::AGE_GREATER:
				return living.age >= protein.parameter; 
			default:
				return true;
		}
	}

	Gene Gene::random()
	{
		Gene gene;
		return random(gene);
	}

	Gene Gene::random(Gene gene)
	{
		for (auto& protein : gene.proteins)
			if (Random::get<bool>(0.4f))
			{
				protein.predicate = Random::get({
						Predicate::NONE, Predicate::NONE, Predicate::NONE,
						Predicate::ENERGY_LESS, Predicate::ENERGY_GREATER, 
						Predicate::HEIGHT_LESS, Predicate::HEIGHT_GREATER,
						Predicate::AGE_LESS, Predicate::AGE_GREATER
					});
				protein.parameter = Random::get(0, 30);
				protein.nextGene = Random::get(0, (Genom::num_genes - 1) << 1);
			}
		return gene;
	}

	Genom::Genom()
	{
		for (byte i = 0; i < num_genes; i++)
			genes[i] = Gene::random();
	}

	Genom Genom::clone() const
	{
		Genom copy{*this};
		/* Mutation! */
		if (Random::get<bool>(mutation_chance))
		{
			auto it = Random::get(copy.genes);
			*it = Gene::random(*it);
		}
		return copy;
	}

	float Genom::mutation_chance = 0.25f;

	bool Genom::grows(World& world, const Cell& cell, Direction dir, const Vector2& pos) const
	{
		return genes[cell.activeGene].grows(world, cell.parent, dir, pos);
	}
}
