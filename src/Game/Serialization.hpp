#pragma once

#include "Genetic.hpp"
#include "Tree.hpp"

#include <nlohmann/json_fwd.hpp>

namespace game
{
	using nlohmann::json;

	void to_json(json& j, const Vector2& v);
	void to_json(json& j, const Living& l);
	void to_json(json& j, const Falling& f);
	void to_json(json& j, const Cell& c);
	void to_json(json& j, const Gene::Protein& protein);
	void to_json(json& j, const Gene& gene);
	void to_json(json& j, const Genom& genom);
	void to_json(json& j, const Tree& tree);
	
	json serialize_tree(const World& world, entt::entity tree);
	json serialize(const entt::handle& object);
}
