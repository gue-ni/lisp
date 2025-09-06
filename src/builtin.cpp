#include "builtin.h"
#include "expr.h"
#include "lisp.h"

namespace lisp
{

namespace builtin
{
Expr * print( void * param, Context & context, const IO & io, Expr * arg )
{
   if( !has_type( arg, Expr::EXPR_CONS ) )
   {

      return make_error( "invalid-type" );
   }
   Expr * expr = eval( arg->cons.car, context, io );
   print_expr( expr, io );
   return make_void();
}

Expr * define( void * param, Context & context, const IO & io, Expr * arg )
{
   // TODO
   assert_type( arg, Expr::EXPR_CONS );

   Expr * car = arg->cons.car;
   assert_type( car, Expr::EXPR_ATOM );
   assert_type( &car->atom, Atom::ATOM_SYMBOL );

   Expr * cdr = arg->cons.cdr;
   assert_type( cdr, Expr::EXPR_CONS );

   Expr * expr = eval( cdr->cons.car, context, io );
   context.set( car->atom.symbol,  expr);
   return make_void();
}

Expr * quote( void * param, Context & context, const IO & io, Expr * arg )
{
   return arg;
}

Expr * add( void * param, Context & context, const IO & io, Expr * arg )
{
   assert( arg->type == Expr::EXPR_CONS );

   double result  = 0;
   Expr * current = arg;

   while( ( has_type( current, Expr::EXPR_CONS ) ) && has_type( current->cons.car, Expr::EXPR_ATOM )
          && ( current->cons.car->atom.type == Atom::ATOM_NUMBER ) )
   {

      Expr * tmp = eval( current->cons.car, context, io );

      assert_type( tmp, Expr::Type::EXPR_ATOM );
      assert_type( &tmp->atom, Atom::Type::ATOM_NUMBER );

      result += tmp->atom.number;
      current = current->cons.cdr;
   }

   return make_number( result );
}

Expr * mult( void * param, Context & context, const IO & io, Expr * arg )
{
   assert( arg->type == Expr::EXPR_CONS );

   double result  = 1;
   Expr * current = arg;

   while( ( current->type == Expr::EXPR_CONS ) && ( current->cons.car->type == Expr::EXPR_ATOM )
          && ( current->cons.car->atom.type == Atom::ATOM_NUMBER ) )
   {
      result *= current->cons.car->atom.number;
      current = current->cons.cdr;
   }

   return make_number( result );
}

} // namespace builtin

} // namespace lisp