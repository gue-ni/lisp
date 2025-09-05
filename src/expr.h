#pragma once

#include <cassert>
namespace lisp {

///////////////////////////////////////////////////////////////////////////////

struct Expr;

///////////////////////////////////////////////////////////////////////////////

struct Cons {
  Expr *car;
  Expr *cdr;
  Cons(Expr *a, Expr *b) : car(a), cdr(b) {}
};

inline Expr *car(Cons *c) { return c->car; }

inline Expr *cdr(Cons *c) { return c->cdr; }

///////////////////////////////////////////////////////////////////////////////

enum ExprType {
  NIL,
  SYMBOL,
  NUMBER,
  CONS,
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
  Expr(Cons c) : type(CONS), cons(c) {}
};

inline Expr *make_nil() { return new Expr(); }

inline Expr *make_cons(Expr *a, Expr *b) { return new Expr(Cons(a, b)); }

inline Expr *make_number(double num) { return new Expr(num); }

///////////////////////////////////////////////////////////////////////////////

inline Expr *car(Expr *e) {
  assert(e->type == CONS);
  return e->cons.car;
}

inline Expr *cdr(Expr *e) {
  assert(e->type == CONS);
  return e->cons.cdr;
}

} // namespace lisp