#pragma once

#include "Components.hpp"

#include <array>

namespace game
{
	class World;

	struct Gene
	{
		enum class Predicate : byte
		{
			NONE,
			ENERGY_LESS,
			ENERGY_GREATER,
			HEIGHT_LESS,
			HEIGHT_GREATER,
			AGE_LESS,
			AGE_GREATER,
			MAX
		};
	
		struct Protein
		{
			Predicate predicate = Predicate::NONE;
			byte parameter = 0;
			byte nextGene = 30;
		};

		std::array<Protein, 4> proteins;

		inline auto& up() { return proteins[static_cast<byte>(Direction::UP)]; }
		inline auto& down() { return proteins[static_cast<byte>(Direction::DOWN)]; }
		inline auto& left() { return proteins[static_cast<byte>(Direction::LEFT)]; }
		inline auto& right() { return proteins[static_cast<byte>(Direction::RIGHT)]; }

		inline auto& up() const { return proteins[static_cast<byte>(Direction::UP)]; }
		inline auto& down() const { return proteins[static_cast<byte>(Direction::DOWN)]; }
		inline auto& left() const { return proteins[static_cast<byte>(Direction::LEFT)]; }
		inline auto& right() const { return proteins[static_cast<byte>(Direction::RIGHT)]; }

		bool grows(const World& world, entt::entity tree, Direction dir, const Vector2& pos) const;

		/* generate a random gene */
		static Gene random();
		/* generate a random gene by mutating an existing one */
		static Gene random(Gene gene);
	};

	class Genom
	{
	public:
		static constexpr byte num_genes = 16;
		static float mutation_chance;

	private:
		std::array<Gene, num_genes> genes;

	public:
		Genom();

		Genom clone() const;

		auto& getGenes() const noexcept { return genes; }

		[[nodiscard]] 
		bool grows(World& world, const Cell& cell, Direction dir, const Vector2& pos) const;
	};
}

