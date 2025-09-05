#pragma once

#include "builtin.h"
#include "expr.h"
#include "util.h"

#include <iostream>
#include <map>
#include <ostream>
#include <vector>

namespace lisp {

///////////////////////////////////////////////////////////////////////////////

class GC {
public:
  Expr *alloc();

  void mark();
  void sweep();

private:
  std::vector<Expr *> m_heap;
};

///////////////////////////////////////////////////////////////////////////////

class Context {
public:
  std::map<std::string, Expr *> env;
};

///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////

void print(Expr *expr, const IO &io);

Expr *eval(Expr *expr, Context &context, const IO &io);

int eval(const std::string &source, Context &context, const IO &io);

int repl();

} // namespace lisp