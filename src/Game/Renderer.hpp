#pragma once

#include "World.hpp"

#include <entt/entity/registry.hpp>

namespace game
{
	/**
	 * Requirements for `D`:
	 unsigned int width() - returns display's width
	 unsigned int height() - returns display's height
	 void begin() - begins rendering.
	 void end() - ends rendering.
	 void draw(unsigned y, unsigned x, char ch, unsigned color) - draws a tile at position={x, y}
	 colored with `color`. Color codes: 0 - black, 1 - red, 2 - green, 3 - yellow,
	 4 - blue, 5 - magenta, 6 - cyan, 7 - white.
	 void onScroll() - scroll callback
	*/
	template<class D>
	struct Renderer : public D
	{
		using displayer_type = D;

		World& world;
		int x, y;

		Renderer(World& world, const D& d)
			: D(d), world(world), x(0), y(0) {}

		Renderer(World& world, D&& d)
			: D(std::move(d)), world(world), x(0), y(0) {}

		void scroll(int offsetX, int offsetY)
		{
			x = std::min(std::max(minX(), x + offsetX), maxX());
			y = std::min(std::max(minY(), y + offsetY), maxY());
			displayer_type::onScroll();	
		}

		void render(bool energyMode)
		{
			unsigned int w = std::min(unsigned(displayer_type::width()), world.w-x);
			unsigned int h = std::min(unsigned(displayer_type::height()), world.h-y);
			auto& reg = world.registry;
			displayer_type::begin();
			if (energyMode)
			{
				for (unsigned int i = 0; i < w; i++)
				{
					for (unsigned int j = 0; j < h; j++)
					{
						Vector2 pos{x + i, y + j};
						entt::entity entity = world.at(pos);
						if (reg.orphan(entity))
						{
							displayer_type::draw(h - j - 1, i, ' ', 0); /* draw black tile */
						}
						else if (auto pCell = reg.template try_get<Cell>(entity))
						{
							int energy = reg.template get<Tree>(pCell->parent).energy;
							unsigned colorIndex = 4;
							if (energy > 2500) colorIndex = 5;
							else if (energy > 1500) colorIndex = 1;
							else if (energy > 1000) colorIndex = 3;
							else if (energy > 500) colorIndex = 2;
							else if (energy > 150) colorIndex = 6;
							char c = (pCell->type == Cell::Type::ACTIVE) ? '$' : ' ';
							displayer_type::draw(h - j - 1, i, c, colorIndex); /* draw white tile */
						}
						else throw std::runtime_error("Found entity with undefined components!");
					}
				}	
			}
			else
			{
				for (unsigned int i = 0; i < w; i++)
				{
					for (unsigned int j = 0; j < h; j++)
					{
						Vector2 pos{x + i, y + j};
						entt::entity entity = world.at(pos);
						if (reg.orphan(entity))
						{
							displayer_type::draw(h - j - 1, i, ' ', 0); /* draw black tile */
						}
						else if (auto pCell = reg.template try_get<Cell>(entity))
						{
							if (auto pLiving = reg.template try_get<Living>(pCell->parent))	
							{
								/* draw colorful tile */
								if (pCell->type == Cell::Type::ACTIVE)
									displayer_type::draw(h - j - 1, i, '$', pLiving->colorIndex);
								else displayer_type::draw(h - j - 1, i, ' ', pLiving->colorIndex);
							}
							else displayer_type::draw(h - j - 1, i, '$', 7); /* draw white tile */
						}
						else throw std::runtime_error("Found entity with undefined components!");
					}
				}
			}
			displayer_type::end();
		}

		[[nodiscard]]
		constexpr int minX() const { return 0; }
		[[nodiscard]]
		constexpr int minY() const { return 0; }
		[[nodiscard]]
		int maxX() const { return world.w - displayer_type::width(); }
		[[nodiscard]]
		int maxY() const { return world.h - displayer_type::height(); }
	};
}
