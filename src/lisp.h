#pragma once

#include <iostream>
#include <ostream>

namespace lisp {

///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////

struct Expr;

///////////////////////////////////////////////////////////////////////////////

struct Cons {
  Expr *car;
  Expr *cdr;
  Cons(Expr *a, Expr *b) : car(a), cdr(b) {}
};

inline Cons cons(Expr *a, Expr *b) { return Cons(a, b); }

inline Expr *car(Cons *c) { return c->car; }

inline Expr *cdr(Cons *c) { return c->cdr; }

///////////////////////////////////////////////////////////////////////////////

enum ExprType {
  NIL,
  SYMBOL,
  NUMBER,
  LIST,
};

///////////////////////////////////////////////////////////////////////////////

struct Expr {
  ExprType type;
  union {
    double number;
    Cons cons;
  };

  Expr() : type(NIL) {}
  Expr(double v) : type(NUMBER), number(v) {}
  Expr(Cons c) : type(LIST), cons(c) {}
};

inline Expr *make_nil() { return new Expr(); }

inline Expr *make_list(Expr *a, Expr *b) { return new Expr(cons(a, b)); }

inline Expr *make_number(double num) { return new Expr(num); }

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

void print(Expr *expr, const IO &io) ;

Expr *eval(Expr *expr, Context &context, const IO &io);

int eval(const std::string &source, Context &context, const IO &io);

int repl();

} // namespace lisp