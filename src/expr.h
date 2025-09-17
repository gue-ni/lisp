#pragma once

#include "builtin.h"
#include "gc.h"

#include <cassert>
#include <cstring>
#include <list>
#include <vector>

#ifdef __unix__
#define STRDUP strdup
#else
#define STRDUP _strdup
#endif

namespace lisp
{

///////////////////////////////////////////////////////////////////////////////

struct Expr;

///////////////////////////////////////////////////////////////////////////////

struct LambdaFn
{
   Expr * params;
   Expr * body;
   Context * env;
   LambdaFn( Expr * p, Expr * b, Context * e )
       : params( p )
       , body( b )
       , env( e )
   {
   }
};

///////////////////////////////////////////////////////////////////////////////

struct Atom
{
   enum Type
   {
      ATOM_NIL,
      ATOM_BOOLEAN,
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
      bool boolean;
      double number;
      char * symbol;
      char * string;
      char * error;
      LambdaFn lambda;
      NativeFunction native;
   };

   ~Atom();
   Atom();
   Atom( Atom && other ) noexcept;

   bool is_truthy() const;

   void print( const IO & io ) const;
   std::string to_json() const;

   bool operator==( const Atom & other ) const;
   bool operator>( const Atom & other ) const;
};

///////////////////////////////////////////////////////////////////////////////

struct Cons
{
   Expr * car; // data
   Expr * cdr; // next

   Cons( Expr * _car, Expr * _cdr );

   void print( const IO & io ) const;
   std::string to_json() const;
};

///////////////////////////////////////////////////////////////////////////////

struct Expr : public gc::Garbage
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

   ~Expr();
   Expr();
   Expr( Atom && a );
   Expr( Cons c );

   void print( const IO & io ) const;
   std::string to_json() const;

   bool is_void() const;
   bool is_cons() const;
   bool is_atom() const;

   bool is_nil() const;
   bool is_string() const;
   bool is_number() const;
   bool is_symbol( const char * symbol ) const;
   bool is_lambda() const;
   bool is_native() const;
   bool is_error() const;
   bool is_truthy() const;

   void mark() override;
};

///////////////////////////////////////////////////////////////////////////////

inline Expr * make_void()
{
   return gc::alloc<Expr>();
}

inline Expr * make_expr( Atom && atom )
{
   return gc::alloc<Expr>( std::move( atom ) );
}

inline Expr * make_expr( Cons cons )
{
   return gc::alloc<Expr>( cons );
}

inline Expr * make_nil()
{
   return gc::alloc<Expr>( Atom() );
}

inline Expr * make_cons( Expr * a, Expr * b )
{
   Cons cons( a, b );
   return make_expr( cons );
}

inline Expr * make_boolean( bool boolean )
{
   Atom atom;
   atom.type    = Atom::ATOM_BOOLEAN;
   atom.boolean = boolean;
   return make_expr( std::move( atom ) );
}

inline Expr * make_number( double number )
{
   Atom atom;
   atom.type   = Atom::ATOM_NUMBER;
   atom.number = number;
   return make_expr( std::move( atom ) );
}

inline Expr * make_symbol( const char * symbol )
{
   Atom atom;
   atom.type   = Atom::ATOM_SYMBOL;
   atom.symbol = STRDUP( symbol );
   return make_expr( std::move( atom ) );
}

inline Expr * make_error( const char * error )
{
   Atom atom;
   atom.type  = Atom::ATOM_ERROR;
   atom.error = STRDUP( error );
   return make_expr( std::move( atom ) );
}

inline Expr * make_string( const char * string )
{
   Atom atom;
   atom.type   = Atom::ATOM_STRING;
   atom.string = STRDUP( string );
   return make_expr( std::move( atom ) );
}

inline Expr * make_native( NativeFunction fn )
{
   Atom atom;
   atom.type   = Atom::ATOM_NATIVE;
   atom.native = fn;
   return make_expr( std::move( atom ) );
}

inline Expr * make_lambda( Expr * params, Expr * body, Context * env )
{
   Atom atom;
   atom.type   = Atom::ATOM_LAMBDA;
   atom.lambda = LambdaFn( params, body, env );
   return make_expr( std::move( atom ) );
}

inline Expr * make_list()
{
   return make_nil();
}

template <typename Car, typename... Cdr>
inline Expr * make_list( Car car, Cdr... cdr )
{
   return make_cons( car, make_list( cdr... ) );
}

///////////////////////////////////////////////////////////////////////////////

} // namespace lisp