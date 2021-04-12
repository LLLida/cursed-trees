
#include "Game/World.hpp"
#include "Game/Display.hpp"
#include "Game/Serialization.hpp"
#include "Graphics/Widgets.hpp"

#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>
#include <fstream>
#include <fmt/chrono.h>
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

/// globals
unsigned int worldW = 0, worldH = 0;
bool running = true;
Mode mode = Mode::IDLE;
int numTicks = 0;

int main(int argc, char** argv)
{
	if (argc == 3)
	{
		worldW = std::stoi(argv[1]);
		worldH = std::stoi(argv[2]);
	}

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
	if (worldH == 0) worldH = static_cast<unsigned int>(window.height());

	entt::registry registry;
	game::World world{registry, worldW, worldH};
	game::Display<game::World> displayer{world, window};

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

	displayer.pBlackPair = &black;
	displayer.pRedPair = &red;
	displayer.pGreenPair = &green;
	displayer.pYellowPair = &yellow;
	displayer.pBluePair = &blue;
	displayer.pMagentaPair = &magenta;
	displayer.pCyanPair = &cyan;
	displayer.pWhitePair = &white;
	displayer.scrollCallback = [&]() { 
		header.properties[30] = fmt::format("Position:[{:3}, {:3}]", displayer.x, displayer.y);
	};
	displayer.scroll(0, 0); /* show 'position' property by calling scrollCallback */

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
				case graphics::Key::s:
					for (int i = 0; i < 100; i++)
					{
						world.tick();
						numTicks++;
					}
					endline.print("Skipped 100 years.");
					break;
				case graphics::Key::S:
					endline.print("Please, wait...");
					endline.draw();
					for (int i = 0; i < 1000; i++)
					{
						world.tick();
						numTicks++;
					}
					endline.print("Skipped 1000 years.");
					break;
				case graphics::Key::i:
					if (displayer.y == displayer.maxY()) 
					{
						endline.print("Top of world.");
						graphics::beep();
					}
					displayer.scroll(0, 1);
					break;
				case graphics::Key::k:
					if (displayer.y == displayer.minY()) 
					{
						endline.print("Bottom of world.");
						graphics::beep();
					}
					displayer.scroll(0, -1);
					break;
				case graphics::Key::j:
					if (displayer.x == displayer.minX()) 
					{
						endline.print("Beginning of world.");
						graphics::beep();
					}
					displayer.scroll(-1, 0);
					break;
				case graphics::Key::l:
					if (displayer.x == displayer.maxX()) 
					{
						endline.print("End of world.");
						graphics::beep();
					}
					displayer.scroll(1, 0);
 					break;
			case graphics::Key::u:
					if (displayer.x < 10) 
					{
						endline.print("Beginning of world.");
						graphics::beep();
					}
					displayer.scroll(-10, 0);
 					break;
			case graphics::Key::o:
				if (displayer.x > displayer.maxX()-10) 
					{
						endline.print("End of world.");
						graphics::beep();
					}
					displayer.scroll(10, 0);
 					break;
				default:
					break;
			}
		}

		if (mode == Mode::TICK)
		{
			header.properties[0] = fmt::format("Trees:[{:4}]", registry.size<game::Tree>());
			header.properties[15] = fmt::format("Year:[{:5}]", numTicks++);
			if (!world.tick())
			{
				endline.on(magenta);
				endline.print("No life");
			}
		}

		displayer();
		gamescreen.draw();
		std::this_thread::sleep_for(wait_time);
	}

	endline.print("Ended in {}", duration_cast<seconds>(high_resolution_clock::now() - start_time));
	endline.draw(red);

	scr.nodelay(false);
	scr.getkey();
		
	graphics::shutdown();
	return 0;
}
