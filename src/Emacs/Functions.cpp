#include "Functions.hpp"

#include "../Game/World.hpp"
#include "../Game/Renderer.hpp"

#include <entt/entity/registry.hpp>
#include <optional>

namespace module
{
  class EmacsDisplayer
  {
  private:
	emacs::env* pEnv;
	emacs::value Qwidth;
	emacs::value Qheight;
	emacs::value Qset_square;
	std::array<emacs::value, 8> Qcolors;

  public:
	EmacsDisplayer(emacs::env& env) {
	  setEnv(env);
	}

	void setEnv(emacs::env& env) {
	  pEnv = &env;
	  Qwidth = env.intern("cursed-trees/screen-width");
	  Qheight = env.intern("cursed-trees/screen-height");
	}

	int width() const
	{
	  return pEnv->extract_integer(pEnv->funcall(Qwidth, 0, nullptr));
	}

	int height() const
	{
	  return pEnv->extract_integer(pEnv->funcall(Qheight, 0, nullptr));
	}

	void draw(unsigned y, unsigned x, char ch, unsigned color)
	{
	  emacs::value Qy = pEnv->make_integer(y);
	  emacs::value Qx = pEnv->make_integer(x);
	  emacs::value Qch = pEnv->make_string(&ch, 1);
	  emacs::value Qfg = (color == 7) ? Qcolors[0] : Qcolors[7];
	  emacs::value Qbg = Qcolors[color];
	  std::array args{Qx, Qy, Qch, Qfg, Qbg};
	  pEnv->funcall(Qset_square, args);
	}

	void onScroll() {}
	void begin() {
	  Qcolors = { 
		pEnv->make_string("black"), pEnv->make_string("red"),
		pEnv->make_string("green"), pEnv->make_string("yellow"), 
		pEnv->make_string("blue"), pEnv->make_string("magenta"),
		pEnv->make_string("cyan"), pEnv->make_string("white")
	  };
	  Qset_square = pEnv->intern("cursed-trees/set-square");
	}
	void end() {}
  };

  static std::optional<entt::registry> gRegistry; /* gWorld must be destroyed after gRegistry */
  static std::optional<game::World> gWorld;
  static std::optional<game::Renderer<EmacsDisplayer>> gRenderer;
  static int gTicks;
  static game::Vector2 gScreenPos{0, 0};

  static void
  error(emacs::env& env, std::string_view message) noexcept
  {
	emacs::value Qerror = env.intern("error");
	env.funcall(Qerror, env.make_string(message));
  }

  /**
   * Evals and then extracts integer.
   * This function can be used with Lisp symbols.
   */
  static int
  extract_integer(emacs::env& env, emacs::value symbol) noexcept
  {
	emacs::value Qeval = env.intern("eval");
	return env.extract_integer(env.funcall(Qeval, symbol));
  }

  static void
  render(emacs::env& env) noexcept
  {
	gRenderer->setEnv(env);
	emacs::value Qenergy_mode = env.intern_and_eval("cursed-trees/energy-mode");
	gRenderer->render(env.is_not_nil(Qenergy_mode));
  }

  emacs::value
  Fcreate_world(emacs::env& env, ptrdiff_t nargs, emacs::value* args) noexcept
  {
	if (nargs != 0 && nargs != 2) {
	  error(env, "Invalid number of arguments(must be 0 or 2)");
	  return env.intern("nil");
	}
	unsigned int width = 200, height = 60;
	if (nargs == 2) {
	  width = env.extract_integer(args[0]);
	  height = env.extract_integer(args[1]);
	}
	gRegistry = entt::registry();
	gWorld = game::World(*gRegistry, width, height);
	gRenderer.emplace(*gWorld, EmacsDisplayer(env));
	gTicks = 0;
	gScreenPos = {0, 0};
	int numTrees = extract_integer(env, env.intern("cursed-trees/num-trees"));
	int initialEnergy = extract_integer(env, env.intern("cursed-trees/initial-energy"));
	if (numTrees > 0) {
	  for (unsigned i = 0; numTrees > 0; numTrees--)
		game::Tree::spawn(*gWorld, i += 5, initialEnergy);
	}
	else error(env, "cursed-trees/num-trees must be a positive value");
	return env.intern("t");
  }

  emacs::value Fdestroy_world(emacs::env& env, ptrdiff_t, emacs::value*) noexcept
  {
	gWorld = std::nullopt;
	if (gRegistry) gRegistry->clear();
	gRegistry = std::nullopt;
	return env.intern("t");
  }

  emacs::value
  Ftick(emacs::env& env, ptrdiff_t nargs, emacs::value* args) noexcept
  {
	if (!gWorld) {
	  error(env, "World is not created, call cursed-trees/create-world firstly");
	  return env.intern("nil");
	}
	unsigned int numIterations = 1;
	if (nargs > 0)
	  numIterations = env.extract_integer(args[0]);
	int sunEnergy = extract_integer(env, env.intern("cursed-trees/sun-energy"));
	int numTicks = 0;
	while (numIterations--) {
	  gWorld->tick(sunEnergy, 3);
	  numTicks++;
	  gTicks++;
	}
	render(env);
	return env.make_integer(numTicks);
  }

  emacs::value Fcurrent_year(emacs::env& env, ptrdiff_t, emacs::value*) noexcept
  {
	if (!gWorld) {
	  error(env, "World is not created, call cursed-trees/create-world firstly");
	  return env.intern("nil");
	}
	return env.make_integer(gTicks);
  }

  emacs::value Fscroll(emacs::env& env, ptrdiff_t, emacs::value* args) noexcept
  {
	if (!gRenderer) {
	  error(env, "World is not created, call cursed-trees/create-world firstly");
	  return env.intern("nil");
	}
	int dX = env.extract_integer(args[0]);
	int dY = env.extract_integer(args[1]);
	gRenderer->setEnv(env);
	gRenderer->scroll(dX, dY);
	render(env);
	std::array newPos{env.make_integer(gRenderer->x), env.make_integer(gRenderer->y)};
	return env.funcall(env.intern("list"), newPos);	/* (list gRenderer->x gRenderer->y)*/
  }
}
