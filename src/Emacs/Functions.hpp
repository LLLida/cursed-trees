#pragma once

#include <emacs-module.hpp>

namespace module
{
  emacs::value Fcreate_world(emacs::env& env, ptrdiff_t nargs, emacs::value* args) noexcept;
  emacs::value Fdestroy_world(emacs::env& env, ptrdiff_t nargs, emacs::value* args) noexcept;
  emacs::value Ftick(emacs::env& env, ptrdiff_t nargs, emacs::value* args) noexcept;
  emacs::value Fcurrent_year(emacs::env& env, ptrdiff_t nargs, emacs::value* args) noexcept;
  emacs::value Fscroll(emacs::env& env, ptrdiff_t nargs, emacs::value* args) noexcept;
  emacs::value Fnum_trees(emacs::env& env, ptrdiff_t nargs, emacs::value* args) noexcept;
}
