#include "Display.hpp"
#include "World.hpp"

#include <entt/entity/registry.hpp>

namespace game
{
	Display<World>::Display(World& world, graphics::Window& win)
		: world(world), window(win), x(0), y(0) {}

	void Display<World>::operator()()
	{
		unsigned int w = static_cast<unsigned int>(window.width());
		unsigned int h = static_cast<unsigned int>(window.height());
		auto& reg = world.registry;
		for (unsigned int i = 0; i < w; i++)
		{
			for (unsigned int j = 0; j < h; j++)
			{
				Vector2 pos{x + i, y + j};
				entt::entity entity = world.at(pos);
				if (reg.orphan(entity))
				{
					if (pBlackPair)
					{
						window.on(*pBlackPair);
						window.mvaddchar(h - j - 1, i, ' ');
						window.off(*pBlackPair);
					}
					else window.mvaddchar(h - j - 1, i, ' ');
				}
				else if (auto pCell = reg.try_get<Cell>(entity))
				{
					graphics::ColorPair* pColorPair = nullptr;
					if (auto pLiving = reg.try_get<Living>(pCell->parent))
						switch(pLiving->colorIndex)
						{
							case 1: pColorPair = pRedPair;
								break;
							case 2: pColorPair = pGreenPair;
								break;
							case 3: pColorPair = pYellowPair;
								break;
							case 4: pColorPair = pBluePair;
								break;
							case 5: pColorPair = pMagentaPair;
								break;
							case 6: pColorPair = pCyanPair;
								break;
							default:
								throw std::runtime_error("Found tree with undefined color index!");
						}
					else pColorPair = pWhitePair;
					if (pColorPair)
					{
						window.on(*pColorPair);
						if (pCell->type == Cell::Type::ACTIVE)
							window.mvaddchar(h - j - 1, i, '$');
						if (pCell->type == Cell::Type::DEAD)
							window.mvaddchar(h - j - 1, i, ' ');
						window.off(*pColorPair);
					}
					else window.mvaddchar(h - j - 1, i, ' ');
				}
				else throw std::runtime_error("Found entity with undefined components!");
			}
		}
	}

	void Display<World>::scroll(int offsetX, int offsetY)
	{
		x = std::min(std::max(minX(), x + offsetX), maxX());
		y = std::min(std::max(minY(), y + offsetY), maxY());
		if (scrollCallback) scrollCallback();
	}

	int Display<World>::maxX() const
	{
		return int(world.w) - window.width();
	}

	int Display<World>::maxY() const
	{
		return int(world.h) - window.height();
	}
}
