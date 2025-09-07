#pragma once

#include "builtin.h"
#include <cassert>
#include <cstring>


#ifdef __POSIX_VERSION
#define STRDUP strdup
#else
#define STRDUP _strdup
#endif

namespace lisp
{

///////////////////////////////////////////////////////////////////////////////

struct Expr;

///////////////////////////////////////////////////////////////////////////////

struct Cons
{
   Expr * car; // data
   Expr * cdr; // next
   Cons( Expr * a, Expr * b )
       : car( a )
       , cdr( b )
   {
   }
};

inline Expr * car( Cons * c )
{
   return c->car;
}

inline Expr * first( Cons * c )
{
   return car( c );
}

inline Expr * cdr( Cons * c )
{
   return c->cdr;
}

inline Expr * rest( Cons * c )
{
   return cdr( c );
}

///////////////////////////////////////////////////////////////////////////////

struct Symbol
{
   char * lexeme;
   Symbol( const char * str )
   {
      lexeme = STRDUP( str );
   }
};

///////////////////////////////////////////////////////////////////////////////

struct NativeFn
{
   NativeFunction fn;
   Expr * operator()( Expr * args, Context & context, const IO & io );
};

///////////////////////////////////////////////////////////////////////////////

struct LambdaFn
{
   Expr * params;
   Expr * body;
   LambdaFn( Expr * p, Expr * bdy )
       : params( p )
       , body( bdy )
   {
   }
   Expr * operator()( Expr * args, Context & context, const IO & io );
};

///////////////////////////////////////////////////////////////////////////////

struct Atom
{
   enum Type
   {
      ATOM_NIL,
      ATOM_NUMBER,
      ATOM_SYMBOL,
      ATOM_STRING,
      ATOM_LAMBDA,
      ATOM_NATIVE,
      ATOM_ERROR,
   };

   Type type;
   union
   {
      double number;
      char * symbol;
      char * string;
      char * error;
      LambdaFn lambda;
      NativeFn native;
   };

   Atom()
       : type( ATOM_NIL )
   {
   }
};

///////////////////////////////////////////////////////////////////////////////

struct Expr
{

   enum Type
   {
      EXPR_VOID,
      EXPR_ATOM,
      EXPR_CONS,
   };

   Type type;

   union
   {
      Atom atom;
      Cons cons;
   };

   Expr()
       : type( EXPR_VOID )
   {
   }

   Expr( Atom a )
       : type( EXPR_ATOM )
       , atom( a )
   {
   }
   Expr( Cons c )
       : type( EXPR_CONS )
       , cons( c )
   {
   }

   bool is_void() const;
   bool is_cons() const;
   bool is_atom() const;
};

///////////////////////////////////////////////////////////////////////////////
// TODO: add garbage collection
inline Expr * make_void()
{
   return new Expr();
}

inline Expr * make_expr( Atom atom )
{
   return new Expr( atom );
}

inline Expr * make_nil()
{
   return new Expr( Atom() );
}

inline Expr * make_cons( Expr * a, Expr * b )
{
   return new Expr( Cons( a, b ) );
}

inline Expr * make_number( double number )
{
   Atom atom;
   atom.type   = Atom::ATOM_NUMBER;
   atom.number = number;
   return new Expr( atom );
}

inline Expr * make_symbol( const char * symbol )
{
   Atom atom;
   atom.type   = Atom::ATOM_SYMBOL;
   atom.symbol = STRDUP( symbol );
   return new Expr( atom );
}

inline Expr * make_error( const char * error )
{
   Atom atom;
   atom.type  = Atom::ATOM_ERROR;
   atom.error = STRDUP( error );
   return new Expr( atom );
}

inline Expr * make_string( const char * string )
{
   Atom atom;
   atom.type   = Atom::ATOM_STRING;
   atom.string = STRDUP( string );
   return new Expr( atom );
}

inline Expr * make_native( NativeFunction fn )
{
   Atom atom;
   atom.type   = Atom::ATOM_NATIVE;
   atom.native = NativeFn{ fn };
   return new Expr( atom );
}

inline Expr * make_lambda( Expr * params, Expr * body )
{
   Atom atom;
   atom.type   = Atom::ATOM_LAMBDA;
   atom.lambda = LambdaFn( params, body );
   return new Expr( atom );
}

inline bool has_type( const Expr * e, Expr::Type t )
{
   return e->type == t;
}

inline bool has_type( const Atom * a, Atom::Type t )
{
   return a->type == t;
}

inline bool has_type( const Atom & a, Atom::Type t )
{
   return a.type == t;
}

inline void assert_type( const Expr * e, Expr::Type t )
{
   assert( has_type( e, t ) );
}

inline void assert_type( const Atom * a, Atom::Type t )
{
   assert( has_type( a, t ) );
}

inline bool is_symbol( Expr * expr, const char * symbol )
{
   return ( expr->type == Expr::EXPR_ATOM ) && ( expr->atom.type == Atom::ATOM_SYMBOL )
          && ( strcmp( expr->atom.symbol, symbol ) == 0 );
}

inline bool is_nil( Expr * expr )
{
   return ( expr->type == Expr::EXPR_ATOM ) && ( expr->atom.type == Atom::ATOM_NIL );
}

///////////////////////////////////////////////////////////////////////////////

inline Expr * car( Expr * e )
{
   assert_type( e, Expr::EXPR_CONS );
   return e->cons.car;
}

inline Expr * cdr( Expr * e )
{
   assert_type( e, Expr::EXPR_CONS );
   return e->cons.cdr;
}

///////////////////////////////////////////////////////////////////////////////

void print_debug( std::ostream & os, const Expr * expr );

} // namespace lisp