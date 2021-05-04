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
	emacs::value Fset_square;
	std::array<emacs::value, 8> Qcolors;

  public:
	EmacsDisplayer(emacs::env& env)
	  : pEnv(&env)
	  , Qwidth(env.intern("cursed-trees/screen-width"))
	  , Qheight(env.intern("cursed-trees/screen-height"))
	  , Fset_square(env.intern("cursed-trees/set-square"))
	  , Qcolors{ env.make_string("black"), env.make_string("red"),
	             env.make_string("green"), env.make_string("yellow"), 
				 env.make_string("blue"), env.make_string("magenta"),
				 env.make_string("cyan"), env.make_string("white")
		}
	{}

	int width()
	{
	  return pEnv->extract_integer(pEnv->funcall(Qwidth, 0, nullptr));
	}

	int height()
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
	  pEnv->funcall(Fset_square, args);
	}

	void onScroll() {}
	void begin() {}
	void end() {}
  };

  static std::optional<game::World> gWorld;
  static std::optional<entt::registry> gRegistry;
  static int gTicks;

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
	gTicks = 0;
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
	game::Renderer renderer(*gWorld, EmacsDisplayer(env));
	emacs::value Qenergy_mode = env.intern_and_eval("cursed-trees/energy-mode");
	renderer.render(env.is_not_nil(Qenergy_mode));
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
}
