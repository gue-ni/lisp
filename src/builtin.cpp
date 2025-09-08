#include "builtin.h"
#include "eval.h"
#include "expr.h"

namespace lisp
{

namespace builtin
{

///////////////////////////////////////////////////////////////////////////////

Expr * f_print( Expr * arg, Context & context, const IO & io )
{
   if( !has_type( arg, Expr::EXPR_CONS ) )
   {
      return make_error( "invalid-type" );
   }
   Expr * expr = eval( arg->cons.car, context, io );
   expr->print( io );
   io.out << std::endl;
   return make_void();
}

Expr * f_print_debug( Expr * arg, Context & context, const IO & io )
{
   arg->print_debug( io.out );
   io.out << std::endl;
   return make_void();
}

///////////////////////////////////////////////////////////////////////////////

Expr * f_add( Expr * arg, Context & context, const IO & io )
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

Expr * f_sub( Expr * arg, Context & context, const IO & io )
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

Expr * f_mul( Expr * arg, Context & context, const IO & io )
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

Expr * f_div( Expr * arg, Context & context, const IO & io )
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

Expr * f_car( Expr * arg, Context & context, const IO & io )
{
   if( !arg->is_cons() )
   {
      make_error( "car expected 1 argument" );
   }

   //arg->print_debug( io.out );
   //io.out << std::endl;

   // Expr * first = eval( arg->cons.car, context, io );

   return arg->cons.car->cons.car;
}

Expr * f_cdr( Expr * arg, Context & context, const IO & io )
{
   Expr * value = eval( arg->cons.car, context, io );
   return value->cons.cdr;
}

///////////////////////////////////////////////////////////////////////////////

Expr * f_eval( Expr * arg, Context & context, const IO & io )
{
   return eval( arg->cons.car, context, io );
}

} // namespace builtin

} // namespace lisp