#pragma once

#include "builtin.h"
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

class GC
{
 public:
   static void garbage_collection();
   static std::list<Expr *> heap;
   static void mark( Expr * expr );

 private:
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

   ~Atom()
   {
      switch( type )
      {
         case lisp::Atom::ATOM_NIL :
         case lisp::Atom::ATOM_NUMBER :
         case lisp::Atom::ATOM_LAMBDA :
         case lisp::Atom::ATOM_NATIVE :
            break;
         case lisp::Atom::ATOM_SYMBOL :
            if( symbol )
            {
               free( symbol );
            }
            break;
         case lisp::Atom::ATOM_STRING :
            if( string )
            {
               free( string );
            }
            break;
         case lisp::Atom::ATOM_ERROR :
            if( error )
            {
               free( error );
            }
            break;
      }
   }

   Atom( Atom && other ) noexcept
       : type( other.type )
   {
      switch( type )
      {
         case lisp::Atom::ATOM_NIL :
            break;
         case lisp::Atom::ATOM_NUMBER :
            number = other.number;
            break;
         case lisp::Atom::ATOM_SYMBOL :
            symbol       = other.symbol;
            other.symbol = nullptr;
            break;
         case lisp::Atom::ATOM_STRING :
            string       = other.string;
            other.string = nullptr;
            break;
         case lisp::Atom::ATOM_LAMBDA :
            lambda = other.lambda;
            break;
         case lisp::Atom::ATOM_NATIVE :
            native = other.native;
            break;
         case lisp::Atom::ATOM_ERROR :
            error       = other.error;
            other.error = nullptr;
            break;
      }
   }

   void print( const IO & io ) const;
   void print_debug( std::ostream & os ) const;
};

///////////////////////////////////////////////////////////////////////////////

struct Cons
{
   Expr * car; // data
   Expr * cdr; // next
   Cons( Expr * _car, Expr * _cdr )
       : car( _car )
       , cdr( _cdr )
   {
   }

   ~Cons()
   {
   }

   void print( const IO & io ) const;
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
   bool marked;

   ~Expr()
   {
      switch( type )
      {
         case EXPR_ATOM :
            atom.~Atom();
            break;
         case EXPR_CONS :
            // cons.~Cons();
            break;
         default :
            break;
      }
   }

   Expr()
       : type( EXPR_VOID )
       , marked( false )
   {
   }

   Expr( Atom && a )
       : type( EXPR_ATOM )
       , atom( std::move( a ) )
       , marked( false )
   {
   }

   Expr( Cons c )
       : type( EXPR_CONS )
       , cons( c )
       , marked( false )
   {
   }

   void print( const IO & io ) const;
   void print_debug( std::ostream & os ) const;

   bool is_void() const;
   bool is_cons() const;
   bool is_atom() const;

   bool is_nil() const;
   bool is_symbol( const char * symbol ) const;
};

///////////////////////////////////////////////////////////////////////////////

// TODO: add garbage collection
inline Expr * make_void()
{
   Expr * expr = new Expr();
   GC::heap.push_back( expr );
   // printf("alloc Expr(%p)\n", (void*) expr);
   return expr;
}

inline Expr * make_expr( Atom && atom )
{
   Expr * expr = new Expr( std::move( atom ) );
   GC::heap.push_back( expr );
   // printf("alloc Expr(%p)\n", (void*) expr);
   return expr;
}

inline Expr * make_expr( Cons cons )
{
   Expr * expr = new Expr( cons );
   GC::heap.push_back( expr );
   // printf("alloc Expr(%p)\n", (void*) expr);
   return expr;
}

inline Expr * make_nil()
{
   Expr * expr = new Expr( Atom() );
   GC::heap.push_back( expr );
   // printf("alloc Expr(%p)\n", (void*) expr);
   return expr;
}

inline Expr * make_cons( Expr * a, Expr * b )
{
   Cons cons( a, b );
   return make_expr( cons );
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
   // TODO: use move constructor
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
   atom.native = NativeFn{ fn };
   return make_expr( std::move( atom ) );
}

inline Expr * make_lambda( Expr * params, Expr * body )
{
   Atom atom;
   atom.type   = Atom::ATOM_LAMBDA;
   atom.lambda = LambdaFn( params, body );
   return make_expr( std::move( atom ) );
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

} // namespace lisp