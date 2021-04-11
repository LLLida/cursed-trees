#include "Serialization.hpp"
#include "World.hpp"

#include <entt/entity/handle.hpp>
#include <nlohmann/json.hpp>

namespace game
{
	void to_json(json& j, const Vector2& v)
	{
		j = json{{"x", v.x},
				 {"y", v.y}};
	}

	void to_json(json& j, const Living& l)
	{
		j = json{{"color_index", l.colorIndex},
				 {"age", l.age},
				 {"max_age", l.maxAge}};
	}

	void to_json(json& j, const Falling& f)
	{
		j = json{{"velocity", f.velocity}};
	}

	void to_json(json& j, const Cell& c)
	{
		j = json{{"parent", static_cast<std::underlying_type_t<entt::entity>>(c.parent)},
				 {"active_gene", static_cast<unsigned int>(c.activeGene)}};
	}

	void to_json(json& j, const Gene::Protein& protein)
	{
		std::string_view predicate;
		switch(protein.predicate)
		{
			case Gene::Predicate::NONE:
				predicate = "none";
				break;
			case Gene::Predicate::ENERGY_LESS:
				predicate = "energy_less";
				break;
			case Gene::Predicate::ENERGY_GREATER:
				predicate = "energy_greater";
				break;
			case Gene::Predicate::HEIGHT_LESS:
				predicate = "height_less";
				break;
			case Gene::Predicate::HEIGHT_GREATER:
				predicate = "height_greater";
				break;
			case Gene::Predicate::AGE_GREATER:
				predicate = "age_greater";
				break;
			case Gene::Predicate::AGE_LESS:
				predicate = "age_less";
				break;
			default: 
				predicate = "undefined";
				break;
		}
		j = json{{"predicate", predicate},
				 {"parameter", protein.parameter},
				 {"nextGene", protein.nextGene}};
	}

	void to_json(json& j, const Gene& gene)
	{
		j = json{{"up", gene.up()},
				 {"down", gene.down()},
				 {"left", gene.left()},
				 {"right", gene.right()}};
	}

	void to_json(json& j, const Genom& genom)
	{
		j = genom.getGenes();
	}

	void to_json(json& j, const Tree& t)
	{
		j = json{{"energy", t.energy},
				 {"alive_cells", t.aliveCells},
				 {"dead_cells", t.deadCells},
				 {"genom", t.genom}};
	}

	json serialize_tree(const World& world, entt::entity entity)
	{
		auto& reg = world.registry;
		auto& tree = reg.get<Tree>(entity);
		json jAliveCells = tree.aliveCells;
		for (auto it = tree.aliveCells.begin(); auto& node : jAliveCells)
		{
			node["active_gene"] = reg.get<Cell>(world.at(*it)).activeGene;
			it++;
		}
		if (auto pLiving = reg.try_get<Living>(entity))
		{
			return json{{"energy", tree.energy},
						{"genom", tree.genom},
						{"alive_cells", jAliveCells},
						{"dead_cells", tree.deadCells},
						{"living", *pLiving}};
		}
		if (auto pFalling = reg.try_get<Falling>(entity))
		{
			return json{{"energy", tree.energy},
						{"genom", tree.genom},
						{"alive_cells", jAliveCells},
						{"dead_cells", tree.deadCells},
						{"falling", *pFalling}};
		}
		else throw std::runtime_error("Passed invalid tree to serialize");
	}

	json serialize(const entt::handle& object)
	{
		if (object.orphan()) return {};
		else if (auto pCell = object.try_get<Cell>())
			return json{{"color_index", object.registry()->get<Living>(pCell->parent).colorIndex}};
		else if (auto pTree = object.try_get<Tree>())
		{
			if (auto pLiving = object.try_get<Living>())
				return json{{"tree", *pTree},
							{"living", *pLiving}};
			else if (auto pFalling = object.try_get<Falling>())
				return json{{"tree", *pTree},
							{"falling", *pFalling}};
			else throw std::runtime_error("Passed invalid tree to serialize");
		}
		else throw std::runtime_error("Passed undefined entity to serialize");
	}
}
