
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

int main()
{
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

	entt::registry registry;
	game::World world{registry, 
		static_cast<unsigned int>(window.width()), 
		static_cast<unsigned int>(window.height())};
	game::Display<game::World> displayer{world};

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

	for (int i = 1; i < 13; i++)
		game::Tree::spawn(world, i * 10);

	enum class Mode {
		IDLE,
		TICK
	};
	bool running = true;
	Mode mode = Mode::IDLE;
	int numTicks = 0;
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

		displayer(window);
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
