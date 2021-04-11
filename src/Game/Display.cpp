#include "Display.hpp"
#include "World.hpp"

#include <entt/entity/registry.hpp>

namespace game
{
	Display<World>::Display(World& world)
		: world(world), x(0), y(0) {}

	void Display<World>::operator()(graphics::Window& win)
	{
		unsigned int w = std::min(static_cast<unsigned int>(win.width()), world.w);
		unsigned int h = std::min(static_cast<unsigned int>(win.height()), world.h);
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
						win.on(*pBlackPair);
						win.mvaddchar(h - j - 1, i, ' ');
						win.off(*pBlackPair);
					}
					else win.mvaddchar(h - j - 1, i, ' ');
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
						win.on(*pColorPair);
						if (pCell->type == Cell::Type::ACTIVE)
							win.mvaddchar(h - j - 1, i, '$');
						if (pCell->type == Cell::Type::DEAD)
							win.mvaddchar(h - j - 1, i, ' ');
						win.off(*pColorPair);
					}
					else win.mvaddchar(h - j - 1, i, ' ');
				}
				else throw std::runtime_error("Found entity with undefined components!");
			}
		}
	}
}
