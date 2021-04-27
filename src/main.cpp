
#include "Game/World.hpp"
#include "Game/Serialization.hpp"
#include "Game/Renderer.hpp"
#include "Graphics/Widgets.hpp"

#include <args.hxx>
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>
#include <fstream>
#include <fmt/chrono.h>
#include <fmt/ostream.h>
#include <nlohmann/json.hpp>
#include <thread>

void dump_json(game::World& world)
{
	game::json json;
	for (int i = 0; entt::entity entity : world.registry.view<game::Tree>())
	{
		json["tree" + std::to_string(i)] = game::serialize_tree(world, entity);
		i++;
	}
	std::ofstream out("dump.json");
	out << json.dump(2);
}

enum class Mode {
	IDLE,
	TICK
};

struct CursesDisplayer
{
	CursesDisplayer(graphics::Window& window)
		: window(window), pColorPairs() {}

	graphics::Window& window;
	graphics::ColorPair* pColorPairs[8];

	graphics::ColorPair*& pBlackPair() { return pColorPairs[0]; };
	graphics::ColorPair*& pRedPair() { return pColorPairs[1]; };
	graphics::ColorPair*& pGreenPair() { return pColorPairs[2]; };
	graphics::ColorPair*& pYellowPair() { return pColorPairs[3]; };
	graphics::ColorPair*& pBluePair() { return pColorPairs[4]; };
	graphics::ColorPair*& pMagentaPair() { return pColorPairs[5]; };
	graphics::ColorPair*& pCyanPair() { return pColorPairs[6]; };
	graphics::ColorPair*& pWhitePair() { return pColorPairs[7]; };

	int width() const { return window.width(); }
	int height() const { return window.height(); }
	void begin() {}
	void end() {}
	void draw(unsigned y, unsigned x, char ch, unsigned color)
	{
		auto& colorPair = *pColorPairs[color];
		window.on(colorPair);
		window.mvaddchar(y, x, ch);
		window.off(colorPair);
	}
	std::function<void()> onScroll;
};

/// globals
unsigned int worldW = 0, worldH = 0;
bool running = true;
Mode mode = Mode::IDLE;
int numTicks = 0;
int minSun = 5;
bool energyMode = false;

static int parseArguments(int argc, char** argv);

int main(int argc, char** argv)
{
	if (parseArguments(argc, argv))
		return 0;
	using namespace std::chrono;
	const auto start_time = high_resolution_clock::now();

	graphics::init();
	auto scr = graphics::stdscr();
	scr.keypad(true);
	scr.nodelay(true);

	graphics::Screen gamescreen;
	auto& window = gamescreen.surface;
	auto& header = gamescreen.header_line;
	auto& endline = gamescreen.message_line;

	if (worldW == 0) worldW = 200;
	if (worldH == 0) worldH = 50;

	entt::registry registry;
	game::World world{registry, worldW, worldH};
	game::Renderer renderer{world, CursesDisplayer{window}};

	graphics::ColorPair black(graphics::Color::WHITE, graphics::Color::BLACK),
		red(graphics::Color::WHITE, graphics::Color::RED), 
		green(graphics::Color::WHITE, graphics::Color::GREEN),
		yellow(graphics::Color::WHITE, graphics::Color::YELLOW),
		blue(graphics::Color::WHITE, graphics::Color::BLUE),
		magenta(graphics::Color::WHITE, graphics::Color::MAGENTA),
		cyan(graphics::Color::WHITE, graphics::Color::CYAN),
		white(graphics::Color::BLACK, graphics::Color::WHITE);

	header.on(white);
	header.background(white);
	header.properties[0] = "Press SPACE to begin";
	endline.on(white);
	endline.background(white);
	endline.print("Hello world");

	renderer.pBlackPair() = &black;
	renderer.pRedPair() = &red;
	renderer.pGreenPair() = &green;
	renderer.pYellowPair() = &yellow;
	renderer.pBluePair() = &blue;
	renderer.pMagentaPair() = &magenta;
	renderer.pCyanPair() = &cyan;
	renderer.pWhitePair() = &white;
	renderer.onScroll = [&]() { 
		header.properties[30] = fmt::format("Position:[{:3}, {:3}]", renderer.x, renderer.y);
	};
	renderer.scroll(0, 0); /* show 'position' property by calling onScroll */

	for (int i = 1; i < 13; i++)
		game::Tree::spawn(world, i * 10);

	while(running)
	{
		milliseconds wait_time{200};
		if (auto key = scr.getkey())
		{
			wait_time = milliseconds{15}; /* sleep less when user pressed a key */
			switch(*key)
			{
			case graphics::Key::q:
				running = false;
				break;
			case graphics::Key::RESIZE:
				gamescreen.resize();
				break;
			case graphics::Key::SPACE:
				switch(mode)
				{
					case Mode::IDLE: mode = Mode::TICK; break;
					case Mode::TICK: mode = Mode::IDLE; break;
				}
				break;
			case graphics::Key::D:
				dump_json(world);
				endline.print("Dumped world to dump.json");
				break;
			case graphics::Key::e:
				energyMode = !energyMode;
				if (energyMode) endline.print("Energy mode enabled.");
				else endline.print("Energy mode disabled.");
				break;
			case graphics::Key::s:
				for (int i = 0; i < 100; i++)
				{
					world.tick(minSun, 3);
					numTicks++;
				}
				endline.print("Skipped 100 years.");
				break;
			case graphics::Key::S:
				endline.print("Please, wait...");
				endline.draw();
				for (int i = 0; i < 1000; i++)
				{
					world.tick(minSun, 3);
					numTicks++;
				}
				endline.print("Skipped 1000 years.");
				break;
			case graphics::Key::i:
			case graphics::Key::UP:
				if (renderer.y == renderer.maxY()) 
				{
					endline.print("Top of world.");
					graphics::beep();
				}
				renderer.scroll(0, 1);
				break;
			case graphics::Key::k:
			case graphics::Key::DOWN:
				if (renderer.y == renderer.minY()) 
				{
					endline.print("Bottom of world.");
					graphics::beep();
				}
				renderer.scroll(0, -1);
				break;
			case graphics::Key::j:
			case graphics::Key::LEFT:
				if (renderer.x == renderer.minX()) 
				{
					endline.print("Beginning of world.");
					graphics::beep();
				}
				renderer.scroll(-1, 0);
				break;
			case graphics::Key::l:
			case graphics::Key::RIGHT:
				if (renderer.x == renderer.maxX())
				{
					endline.print("End of world.");
					graphics::beep();
				}
				renderer.scroll(1, 0);
				break;
			case graphics::Key::u:
				if (renderer.x < 10) 
				{
					endline.print("Beginning of world.");
					graphics::beep();
				}
				renderer.scroll(-10, 0);
				break;
			case graphics::Key::o:
				if (renderer.x > renderer.maxX()-10) 
				{
					endline.print("End of world.");
					graphics::beep();
				}
				renderer.scroll(10, 0);
					break;
			case graphics::Key::PLUS:
				minSun++;
				endline.print("Sun's energy is now {}.", minSun);
				break;
			case graphics::Key::MINUS:
				minSun--;
				endline.print("Sun's energy is now {}.", minSun);
				break;
			default:
				break;
			}
		}

		if (mode == Mode::TICK)
		{
			header.properties[0] = fmt::format("Trees:[{:4}]", registry.size<game::Tree>());
			header.properties[15] = fmt::format("Year:[{:5}]", numTicks++);
			if (!world.tick(minSun, 3))
			{
				endline.on(magenta);
				endline.print("No life");
			}
		}

		renderer.render(energyMode);
		gamescreen.draw();
		std::this_thread::sleep_for(wait_time);
	}

	endline.print("Ended in {}.", duration_cast<seconds>(high_resolution_clock::now() - start_time));
	endline.draw(red);

	scr.nodelay(false);
	scr.getkey();
		
	graphics::shutdown();
	return 0;
}

static int parseArguments(int argc, char** argv)
{
	args::ArgumentParser parser(
		"cursed-trees is tree evolution simulation with tui.\n"
		"CONTROLS:\n"
		"   [q]: quit the program\n"
		"   [<space>]: pause the program\n"
		"   [e]: toggle energy mode\n"
		"   [<up arrow>, i]: scroll screen up by 1 char\n"
		"   [<left arrow>, j]: scroll screen left by 1 char\n"
		"   [<down arrow>, k]: scroll screen down by 1 char\n"
		"   [<right arrow>, l]: scroll screen right by 1 char\n"
		"   [u]: scroll screen left by 10 chars\n"
		"   [o]: scroll screen right by 10 chars\n"
        "   [-]: reduce sun's energy by 1\n"
        "   [+]: increase sun's energy by 1\n"
        "   [s]: skip 100 years\n"
        "   [S]: skip 1000 years", 
		"If you encounter any bug please add issue to the github repo: https://github.com/LLLida/cursed-trees\n"
		"or leave me email: 0adilmohammad0@gmail.com.");
	args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
	args::CompletionFlag completion(parser, {"complete"});
	args::Flag printVersion(parser, "version", "Display version message", {'v', "version"});
	args::ValueFlag<int> minSunFlag(parser, "0..20", "Set minimal sun energy", {"min-sun"});
	args::ValueFlag<unsigned int> worldWFlag(parser, "number of chars", "world's width", {'w', "width"});
	args::ValueFlag<unsigned int> worldHFlag(parser, "number of chars", "world's height", {'w', "height"});
	try
	{
		parser.ParseCLI(argc, argv);
	}
	catch (const args::Completion& e)
	{
		fmt::print(e.what());
		return 0;
	}
	catch (const args::Help&)
	{
		fmt::print("{}", parser);
		return 1;
	}
	catch (const args::ParseError& e)
	{
		fmt::print("{}\n{}", e.what(), parser);
		return 1;
    }
	catch (const args::ValidationError& e)
	{
		fmt::print("{}\n{}", e.what(), parser);
	}
	if (printVersion)
	{
		fmt::print(R"(
cursed-trees version is 1.0. Built in {}.
Copyright (C) 2021  Adil Mokhammad

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
)", __DATE__);
		return 1;
	}
	if (minSunFlag) minSun = args::get(minSunFlag);
	if (worldWFlag) worldW = args::get(worldWFlag);
	if (worldHFlag) worldH = args::get(worldHFlag);
	return 0;
}
