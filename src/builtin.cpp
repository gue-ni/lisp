#include "builtin.h"
#include "eval.h"
#include "expr.h"

namespace lisp
{

namespace builtin
{

///////////////////////////////////////////////////////////////////////////////

Expr * print( Expr * arg, Context & context, const IO & io )
{
   if( !has_type( arg, Expr::EXPR_CONS ) )
   {

      return make_error( "invalid-type" );
   }
   Expr * expr = eval( arg->cons.car, context, io );
   expr->print(io);
   return make_void();
}

///////////////////////////////////////////////////////////////////////////////

Expr * plus( Expr * arg, Context & context, const IO & io )
{
   assert( arg->is_cons() );

   double result  = 0;
   Expr * current = arg;

   while( ( current->is_cons() ) && has_type( current->cons.car, Expr::EXPR_ATOM )
          && ( current->cons.car->atom.type == Atom::ATOM_NUMBER ) )
   {
      result += current->cons.car->atom.number;
      current = current->cons.cdr;
   }

   return make_number( result );
}

///////////////////////////////////////////////////////////////////////////////

Expr * minus( Expr * arg, Context & context, const IO & io )
{
   assert( arg->is_cons() );

   double result  = 0;
   Expr * current = arg;

   while( ( current->is_cons() ) && has_type( current->cons.car, Expr::EXPR_ATOM )
          && ( current->cons.car->atom.type == Atom::ATOM_NUMBER ) )
   {
      result -= current->cons.car->atom.number;
      current = current->cons.cdr;
   }

   return make_number( result );
}

///////////////////////////////////////////////////////////////////////////////

Expr * mult( Expr * arg, Context & context, const IO & io )
{
   assert( arg->is_cons() );

   double result  = 1;
   Expr * current = arg;

   while( ( current->is_cons() ) && ( current->cons.car->type == Expr::EXPR_ATOM )
          && ( current->cons.car->atom.type == Atom::ATOM_NUMBER ) )
   {
      result *= current->cons.car->atom.number;
      current = current->cons.cdr;
   }

   return make_number( result );
}

///////////////////////////////////////////////////////////////////////////////

Expr * div( Expr * arg, Context & context, const IO & io )
{
   assert( arg->is_cons() );

   double result  = 1;
   Expr * current = arg;

   while( ( current->is_cons() ) && ( current->cons.car->type == Expr::EXPR_ATOM )
          && ( current->cons.car->atom.type == Atom::ATOM_NUMBER ) )
   {
      result /= current->cons.car->atom.number;
      current = current->cons.cdr;
   }

   return make_number( result );
}

///////////////////////////////////////////////////////////////////////////////

Expr * eval_expr( Expr * arg, Context & context, const IO & io )
{
   return eval( arg->cons.car, context, io );
}

} // namespace builtin

} // namespace lisp