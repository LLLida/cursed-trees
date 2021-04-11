#pragma once

#include "../Graphics/Curses.hpp"

#include <functional>
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
		graphics::Window& window;
		int x, y;
		graphics::ColorPair* pBlackPair = nullptr;
		graphics::ColorPair* pRedPair = nullptr;
		graphics::ColorPair* pGreenPair = nullptr;
		graphics::ColorPair* pYellowPair = nullptr;
		graphics::ColorPair* pBluePair = nullptr;
		graphics::ColorPair* pMagentaPair = nullptr;
		graphics::ColorPair* pCyanPair = nullptr;
		graphics::ColorPair* pWhitePair = nullptr;

		Display(World& world, graphics::Window& win);

		void operator()();

		void scroll(int offsetX, int offsetY);
		std::function<void()> scrollCallback;

		[[nodiscard]]
		constexpr int minX() const { return 0; }
			[[nodiscard]]
			constexpr int minY() const { return 0; }
		[[nodiscard]]
		int maxX() const;
		[[nodiscard]]
		int maxY() const;
	};
}
