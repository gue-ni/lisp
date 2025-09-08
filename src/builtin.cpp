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
   return make_void();
}

Expr * f_println( Expr * arg, Context & context, const IO & io )
{
   ( void ) f_print( arg, context, io );
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

///////////////////////////////////////////////////////////////////////////////

Expr * f_car( Expr * arg, Context & context, const IO & io )
{
   if( !arg->is_cons() )
   {
      make_error( "car expected 1 argument" );
   }
   return arg->cons.car->cons.car;
}

///////////////////////////////////////////////////////////////////////////////

Expr * f_cdr( Expr * arg, Context & context, const IO & io )
{
   if( !arg->is_cons() )
   {
      make_error( "car expected 1 argument" );
   }
   return arg->cons.car->cons.cdr;
}

///////////////////////////////////////////////////////////////////////////////

Expr * f_cons( Expr * arg, Context & context, const IO & io )
{
   Expr * a = arg->cons.car;
   Expr * b = arg->cons.cdr->cons.car;
   return make_cons( a, b );
}

///////////////////////////////////////////////////////////////////////////////

Expr * f_gt( Expr * arg, Context & context, const IO & io )
{
   Expr * a = arg->cons.car;
   Expr * b = arg->cons.cdr->cons.car;

   bool is_gt = false;
   if( ( a->type == b->type ) && a->is_atom() )
   {
      is_gt = ( a->atom ) > ( b->atom );
   }
   else
   {
      is_gt = false;
   }

   return make_boolean( is_gt );
}

///////////////////////////////////////////////////////////////////////////////

Expr * f_eq( Expr * arg, Context & context, const IO & io )
{
   Expr * a = arg->cons.car;
   Expr * b = arg->cons.cdr->cons.car;

   bool is_eq = false;

   if( ( a->type == b->type ) && a->is_atom() )
   {
      is_eq = ( a->atom ) == ( b->atom );
   }
   else
   {
      is_eq = false;
   }

   return make_boolean( is_eq );
}

///////////////////////////////////////////////////////////////////////////////

Expr * f_eval( Expr * arg, Context & context, const IO & io )
{
   return eval( arg->cons.car, context, io );
}

///////////////////////////////////////////////////////////////////////////////

Expr * f_if( Expr * arg, Context & context, const IO & io )
{
   // TODO: type error handling
   Expr * cond      = arg->cons.car;
   Expr * then_expr = arg->cons.cdr->cons.car;
   Expr * else_expr = arg->cons.cdr->cons.cdr->cons.car;

   if( cond->is_truthy() )
   {
      return then_expr;
   }
   else
   {
      return else_expr;
   }
}

///////////////////////////////////////////////////////////////////////////////

} // namespace builtin

} // namespace lisp