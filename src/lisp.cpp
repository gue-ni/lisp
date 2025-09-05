#include "lisp.h"
#include "expr.h"
#include "tokenizer.h"

#include <cstdio>
#include <string>
#include <vector>

namespace lisp {

Tokens tokenize(const std::string &source) { return {}; }

Expr *parse(const Tokens &tokens) { return make_nil(); }

Expr *parse(const std::string &source) { return make_nil(); }

Expr *eval_atom(Expr *atom, Context &context, const IO &io) { return atom; }

Expr *eval_all(Expr *atom, Context &context, const IO &io) { return nullptr; }

Expr *eval(Expr *expr, Context &context, const IO &io) {
  switch (expr->type) {
  case NIL:
  case NUMBER: {
    return eval_atom(expr, context, io);
  }
  case CONS: {
    Expr *car = eval(expr->cons.car, context, io);
    Expr *cdr = eval(expr->cons.cdr, context, io);
    return make_cons(car, cdr);
  }
  default:
    io.err << "unhandled-type" << std::endl;
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
    io.err << "unprintable-expr";
    break;
  }
  }
}

int eval(const std::string &source, Context &context, const IO &io) {

  Tokens tokens = tokenize(source);

  Expr *exp = parse(tokens);
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