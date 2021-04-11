#pragma once

#include "../Graphics/Curses.hpp"

#include <map>

namespace game
{
	class World;

	template<class T>
	struct Display;

	template<>
	struct Display<World>
	{
		World& world;
		int x, y;
		graphics::ColorPair* pBlackPair = nullptr;
		graphics::ColorPair* pRedPair = nullptr;
		graphics::ColorPair* pGreenPair = nullptr;
		graphics::ColorPair* pYellowPair = nullptr;
		graphics::ColorPair* pBluePair = nullptr;
		graphics::ColorPair* pMagentaPair = nullptr;
		graphics::ColorPair* pCyanPair = nullptr;
		graphics::ColorPair* pWhitePair = nullptr;

		Display(World& world);

		void operator()(graphics::Window& win);
	};
}
