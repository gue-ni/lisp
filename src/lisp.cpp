#include "lisp.h"

#include <cstdio>
#include <string>

namespace lisp {

Expr *parse(const std::string &source) { return make_nil(); }

int eval(const std::string &source) { return 0; }

Expr *eval(Expr *expr, Context &context, const IO &io) {
  switch (expr->type) {
  case NIL:
  case NUMBER: {
    return expr;
  }
  case LIST: {
    Cons cons = expr->cons;
    Expr *op = eval(cons.car, context, io);
    return make_nil();
  }
  default:
    io.err << "Unhandled type" << std::endl;
    return make_nil();
  }
}

void print(Expr *expr, const IO &io) {
  switch (expr->type) {
  case NUMBER: {
    io.out << (expr->number);
    break;
  }
  case NIL: {
    io.out << "nil";
    break;
  }
  default: {
    io.err << "unprintable";
    break;
  }
  }
}

int eval(const std::string &source, Context &context, const IO &io) {

  Expr *exp = parse(source);
  if (!exp) {
    return 1;
  }

  Expr *res = eval(exp, context, io);
  if (!res) {
    return 2;
  }

  print(res, io);
  io.out << std::endl;
  return 0;
}

int repl() {

  IO io;
  Context ctx;
  std::string line;

  int res = 0;

  do {
    std::cout << "> ";
    if (!std::getline(std::cin, line)) {
      break;
    }

    res = eval(line, ctx, io);
  } while (res == 0);

  return 0;
}

} // namespace lisp