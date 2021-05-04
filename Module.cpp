#include "emacs-module.hpp"

int plugin_is_GPL_compatible = 1;

static emacs::value
Fmymod_test(emacs_env* env, ptrdiff_t nargs, emacs::value args[], void* data) noexcept
{
  return env->make_integer(env, 23);
}

static void
bind_function(emacs::env& env, const char* name, emacs::value Sfun)
{
  emacs::value Qfset = env.intern("fset");
  emacs::value Qsym = env.intern(name);
  emacs::value args[] = { Qsym, Sfun };
  env.funcall(Qfset, 2, args);
}

static void provide(emacs::env& env, const char* feature)
{
  emacs::value Qfeat = env.intern(feature);
  emacs::value Qprovide = env.intern("provide");
  emacs::value args[] = { Qfeat };
  env.funcall(Qprovide, 1, args);
}

int emacs::module_init(runtime &ert) noexcept
{
  auto& env = ert.get_environment();
  emacs::value fun = env.make_function(0, 0,
									   Fmymod_test,
									   "nihao!",
									   nullptr);
  bind_function(env, "my/function-nihao", fun);
  provide(env, "mymod");
  return 0;
}

