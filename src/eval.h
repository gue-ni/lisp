#pragma once

#include "builtin.h"
#include "expr.h"
#include "util.h"

#include <cstdint>
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

class Context : public gc::Garbage
{
 public:
   Context(Context* parent = nullptr);
   ~Context();
   Expr * lookup( const char * symbol ) const;
   void define( const char * symbol, Expr * expr );
   void print( const IO & io ) const;
   const Env & env() const;
   void mark() override;
   Context * parent() { return m_parent; }

   bool exit;
   int exit_code;

 private:
   Context * m_parent;
   Env m_env;
   void load_runtime();
   void load_stdlib();
  bool is_root() const;
};



///////////////////////////////////////////////////////////////////////////////


Expr * expand( Expr * expr );

Expr * eval( Expr * expr, Context & context, const IO & io );

int eval( const std::string & source, Context & context, const IO & io, Flags flags = 0 );

int eval( const std::string & source );

Expr * eval_program( Expr * program, Context & context, const IO & io );

int repl();

///////////////////////////////////////////////////////////////////////////////

} // namespace lisp
