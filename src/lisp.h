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
  Expr *lookup(const char *symbol);
  void set(const char *symbol, Expr *expr);

  void load_runtime();

private:
  std::map<std::string, Expr *> m_env;
};

///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////

void print(Expr *expr, const IO &io);

Expr *eval(Expr *expr, Context &context, const IO &io);

int eval_print(Expr *expr, Context &context, const IO &io, bool newline = false);

int eval(const std::string &source, Context &context, const IO &io);

void load_runtime(Context &context);

int repl();

void print_expr(Expr *expr, const IO &io);

} // namespace lisp