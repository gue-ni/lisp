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
    ATOM_NIL,
    ATOM_NUMBER,
    ATOM_SYMBOL,
    ATOM_STRING, // TODO
    ATOM_LAMBDA, // TODO
    ATOM_NATIVE, // TODO
  };

  Type type;
  union {
    double number;
    char *symbol;
  };

  Atom() : type(ATOM_NIL) {}
  Atom(const char *sym) : type(ATOM_SYMBOL), symbol(strdup(sym)) {}
  Atom(double num) : type(ATOM_NUMBER), number(num) {}
};

struct Expr {

  enum Type {
    EXPR_ATOM,
    EXPR_CONS,
  };

  Type type;

  union {
    Atom atom;
    Cons cons;
  };

  Expr(Atom a) : type(EXPR_ATOM), atom(a) {}
  Expr(Cons c) : type(EXPR_CONS), cons(c) {}
};

inline Expr *make_nil() { return new Expr(Atom()); }

inline Expr *make_cons(Expr *a, Expr *b) { return new Expr(Cons(a, b)); }

inline Expr *make_number(double num) { return new Expr(Atom(num)); }

inline Expr *make_symbol(const char *symbol) { return new Expr(Atom(symbol)); }

///////////////////////////////////////////////////////////////////////////////

inline Expr *car(Expr *e) {
  assert(e->type == Expr::EXPR_CONS);
  return e->cons.car;
}

inline Expr *cdr(Expr *e) {
  assert(e->type == Expr::EXPR_CONS);
  return e->cons.cdr;
}

} // namespace lisp