#pragma once

#include <cassert>
#include <cstring>

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

struct Symbol {
  char *lexeme;
  Symbol(const char *str) { lexeme = strdup(str); }
};

///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////

struct Atom {
  enum Type {
    ATOM_STRING,
    ATOM_NUMBER,
    ATOM_SYMBOL,
    ATOM_LAMBDA,
    ATOM_NATIVE,
  };

  Type type;
  union {
    char *string;
    double number;
    char *symbol;
  };
};

enum ExprType {
  NIL,
#if 0
  ATOM,
#else
  NUMBER,
  SYMBOL,
  STRING,
#endif
  CONS,
};

struct Expr {
  ExprType type;
  union {
#if 0
    Atom atom;
#else
    Symbol symbol;
    double number;
#endif
    Cons cons;
  };

  Expr() : type(NIL) {}
  Expr(Symbol s) : type(SYMBOL), symbol(s) {}
  Expr(double v) : type(NUMBER), number(v) {}
  Expr(Cons c) : type(CONS), cons(c) {}
};

inline Expr *make_nil() { return new Expr(); }

inline Expr *make_cons(Expr *a, Expr *b) { return new Expr(Cons(a, b)); }

inline Expr *make_number(double num) { return new Expr(num); }

inline Expr *make_symbol(const char *symbol) {
  return new Expr(Symbol(symbol));
}

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