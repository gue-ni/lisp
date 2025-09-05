#pragma once

#include "expr.h"

#include <iostream>
#include <ostream>

namespace lisp {

///////////////////////////////////////////////////////////////////////////////

struct Context {};

///////////////////////////////////////////////////////////////////////////////

struct IO {
  std::ostream &out;
  std::ostream &err;
  IO(std::ostream &o = std::cout, std::ostream &e = std::cerr)
      : out(o), err(e) {}
};

///////////////////////////////////////////////////////////////////////////////

void print(Expr *expr, const IO &io);

Expr *eval(Expr *expr, Context &context, const IO &io);

int eval(const std::string &source, Context &context, const IO &io);

int repl();

} // namespace lisp