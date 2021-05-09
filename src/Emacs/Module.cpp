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

  bind_function(env, "cursed-trees/create-world",
				env.make_function(0, 2, module::Fcreate_world,
                                  "Create world for `cursed-trees'.\n"
                                  "If WIDTH or HEIGHT are nil then use `cursed-trees/world-width' and `cursed-trees/world-height'.\n"
                                  "Return `t' if world was created successfully and `nil' otherwise.\n"
                                  "\n\(fn &optional WIDTH HEIGHT)"));
  bind_function(env, "cursed-trees/destroy-world",
				env.make_function(0, 0, module::Fdestroy_world,
                                  "Destroy world for `cursed-trees'.\n"
                                  "This function won't error even if world is not created.\n"));
  bind_function(env, "cursed-trees/tick",
				env.make_function(0, 1, module::Ftick,
                                  "Tick the simulation skipping 1 year.\n"
                                  "If NUM-TICKS provided then skip NUM-TICKS years.\n"
                                  "Return number of real ticks(trees may die and function will stop).\n"
                                  "\n\(fn &optional NUM-TICKS)"));
  bind_function(env, "cursed-trees/current-year",
				env.make_function(0, 0, module::Fcurrent_year,
                                  "Return number of years(ticks) since creation.\n"));
  bind_function(env, "cursed-trees/scroll",
				env.make_function(2, 2, module::Fscroll,
                                  "Move camera DX cells right and DY cells up.\n"
                                  "Return new position of camera.\n"
                                  "\n\(fn DX DY)"));
  bind_function(env, "cursed-trees/num-trees",
				env.make_function(0, 0, module::Fnum_trees,
                                  "Return number alive of trees in simulation.\n"));

  provide(env, "cursed-trees-module");
  return 0;
}

