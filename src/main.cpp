
#include "Game/World.hpp"
#include "Game/Serialization.hpp"
#include "Game/Renderer.hpp"
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
unsigned int minSun = 5;

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
				if (renderer.y == renderer.maxY()) 
				{
					endline.print("Top of world.");
					graphics::beep();
				}
				renderer.scroll(0, 1);
				break;
			case graphics::Key::k:
				if (renderer.y == renderer.minY()) 
				{
					endline.print("Bottom of world.");
					graphics::beep();
				}
				renderer.scroll(0, -1);
				break;
			case graphics::Key::j:
				if (renderer.x == renderer.minX()) 
				{
					endline.print("Beginning of world.");
					graphics::beep();
				}
				renderer.scroll(-1, 0);
				break;
			case graphics::Key::l:
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

		renderer.render();
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
