#include "Functions.hpp"

int plugin_is_GPL_compatible = 1;

/**
 * Binds lisp function `Sfun` to `name`.
 */
static void
bind_function(emacs::env& env, const char* name, emacs::value Sfun)
{
  emacs::value Qfset = env.intern("fset");
  emacs::value Qsym = env.intern(name);
  std::array args{Qsym, Sfun};
  env.funcall(Qfset, args);
}

static void
provide(emacs::env& env, const char* feature)
{
  emacs::value Qfeat = env.intern(feature);
  emacs::value Qprovide = env.intern("provide");
  env.funcall(Qprovide, Qfeat);
}

int emacs::module_init(runtime &ert) noexcept
{
  auto& env = ert.get_environment();
  emacs::value fun = env.make_function(0, 0,
									   [&](emacs::env& env, ptrdiff_t nargs, emacs::value* args) {
										 return env.make_integer(42);
									   },
									   "nihao!");
  bind_function(env, "my/function-nihao", fun);

  bind_function(env, "cursed-trees/create-world",
				env.make_function(0, 2, module::Fcreate_world, "Create world"));
  bind_function(env, "cursed-trees/destroy-world",
				env.make_function(0, 0, module::Fdestroy_world, "Destroy world"));
  bind_function(env, "cursed-trees/tick",
				env.make_function(0, 1, module::Ftick, "Tick"));
  bind_function(env, "cursed-trees/current-year",
				env.make_function(0, 0, module::Fcurrent_year, "Get number of ticks since creation."));

  provide(env, "cursed-trees-module");
  return 0;
}

