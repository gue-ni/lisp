#pragma once

#include "builtin.h"
#include "expr.h"
#include "util.h"

#include <iostream>
#include <map>
#include <ostream>
#include <vector>

namespace lisp
{

///////////////////////////////////////////////////////////////////////////////

class GC
{
 public:
   Expr * alloc();

   void mark();
   void sweep();

 private:
   std::vector<Expr *> m_heap;
};

///////////////////////////////////////////////////////////////////////////////

class Context
{
 public:
   Context();
   Expr * lookup( const char * symbol );
   void define( const char * symbol, Expr * expr );
   void print(const IO& io);

 private:
   std::map<std::string, Expr *> m_env;
   void load_runtime();
};

///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////

Expr * eval( Expr * expr, Context & context, const IO & io );

int eval_print( Expr * expr, Context & context, const IO & io, bool newline = false );

int eval( const std::string & source, Context & context, const IO & io, bool newline = false );

int repl();

void print_expr( Expr * expr, const IO & io );

} // namespace lisp