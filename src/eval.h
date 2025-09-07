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

using Flags = uint8_t;

constexpr Flags FLAG_NEWLINE     = 1 << 0;
constexpr Flags FLAG_DUMP_TOKENS = 1 << 1;
constexpr Flags FLAG_DUMP_AST    = 1 << 2;
constexpr Flags FLAG_DUMP_ENV    = 1 << 3;

///////////////////////////////////////////////////////////////////////////////

using Env = std::map<std::string, Expr *>;

class Context
{
 public:
   Context();
   ~Context();
   Expr * lookup( const char * symbol );
   void define( const char * symbol, Expr * expr );
   void print( const IO & io );
   const Env & env() const;

 private:
   Context * m_parent_scope;
   Env m_env;
   void load_runtime();
};

///////////////////////////////////////////////////////////////////////////////
// garbage collection
namespace gc
{

void mark( Expr * expr );

void sweep();

void run( Context & context );

} // namespace gc

///////////////////////////////////////////////////////////////////////////////

Expr * eval( Expr * expr, Context & context, const IO & io );

int eval( const std::string & source, Context & context, const IO & io, Flags flags = 0 );

int repl();

///////////////////////////////////////////////////////////////////////////////

} // namespace lisp