#include "builtin.h"
#include "expr.h"
#include <exception>

namespace lisp
{

namespace builtin
{

Expr * add( void * param, Context & context, const IO & io, Expr * arg )
{

   assert( arg->type == Expr::EXPR_CONS );

   Expr * current = arg;

   double result = 0;

   while( ( current->type == Expr::EXPR_CONS ) && ( current->cons.car->type == Expr::EXPR_ATOM )
          && ( current->cons.car->atom.type == Atom::ATOM_NUMBER ) )
   {

      result += current->cons.car->atom.number;
      current = current->cons.cdr;
   }

   // TODO
   return make_number( result );
}

} // namespace builtin

} // namespace lisp