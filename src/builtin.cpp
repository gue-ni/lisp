#include <exception>
#include <fstream>
#include <sstream>

#include "builtin.h"
#include "eval.h"
#include "expr.h"
#include "parser.h"
#include "tokenizer.h"

namespace lisp
{

namespace builtin
{

///////////////////////////////////////////////////////////////////////////////

Expr * f_display( Expr * arg, Context & context, const IO & io )
{
   if( !arg->is_cons() )
   {
      return make_error( "invalid-type" );
   }
   Expr * expr = eval( arg->cons.car, context, io );
   expr->print( io );
   return make_void();
}

Expr * f_displayln( Expr * arg, Context & context, const IO & io )
{
   ( void ) f_display( arg, context, io );
   io.out << std::endl;
   return make_void();
}

Expr * f_print_debug( Expr * arg, Context & context, const IO & io )
{
   io.out << arg->to_json() << std::endl;
   return make_void();
}

///////////////////////////////////////////////////////////////////////////////

Expr * f_add( Expr * arg, Context & context, const IO & io )
{
   assert( arg->is_cons() );

   if( !arg->cons.car->is_number() )
   {
      return make_error( "expected a number" );
   }

   double result = arg->cons.car->atom.number;
   arg           = arg->cons.cdr;

   while( ( arg->is_cons() ) )
   {
      if( !arg->cons.car->is_number() )
      {
         return make_error( "expected a number" );
      }
      result += arg->cons.car->atom.number;
      arg = arg->cons.cdr;
   }

   return make_number( result );
}

///////////////////////////////////////////////////////////////////////////////

Expr * f_sub( Expr * arg, Context & context, const IO & io )
{
   assert( arg->is_cons() );

   if( !arg->cons.car->is_number() )
   {
      return make_error( "expected a number" );
   }

   double result = arg->cons.car->atom.number;
   arg           = arg->cons.cdr;

   while( ( arg->is_cons() ) )
   {
      if( !arg->cons.car->is_number() )
      {
         return make_error( "expected a number" );
      }
      result -= arg->cons.car->atom.number;
      arg = arg->cons.cdr;
   }

   return make_number( result );
}

///////////////////////////////////////////////////////////////////////////////

Expr * f_mul( Expr * arg, Context & context, const IO & io )
{
   assert( arg->is_cons() );

   if( !arg->cons.car->is_number() )
   {
      return make_error( "expected a number" );
   }

   double result = arg->cons.car->atom.number;
   arg           = arg->cons.cdr;

   while( ( arg->is_cons() ) )
   {
      if( !arg->cons.car->is_number() )
      {
         return make_error( "expected a number" );
      }
      result *= arg->cons.car->atom.number;
      arg = arg->cons.cdr;
   }

   return make_number( result );
}

///////////////////////////////////////////////////////////////////////////////

Expr * f_div( Expr * arg, Context & context, const IO & io )
{
   assert( arg->is_cons() );

   if( !arg->cons.car->is_number() )
   {
      return make_error( "expected a number" );
   }

   double result = arg->cons.car->atom.number;
   arg           = arg->cons.cdr;

   while( ( arg->is_cons() ) )
   {
      if( !arg->cons.car->is_number() )
      {
         return make_error( "expected a number" );
      }

      if( arg->cons.car->atom.number == 0 )
      {
         return make_error( "division by zero" );
      }
      result /= arg->cons.car->atom.number;
      arg = arg->cons.cdr;
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

Expr * f_ge( Expr * arg, Context & context, const IO & io )
{
   Expr * a = arg->cons.car;
   Expr * b = arg->cons.cdr->cons.car;

   if( ( a->type != b->type ) && a->is_atom() )
   {
      return make_error( "atom expected" );
   }

   return make_boolean( ( a->atom == b->atom ) || ( a->atom > b->atom ) );
}

///////////////////////////////////////////////////////////////////////////////

Expr * f_lt( Expr * arg, Context & context, const IO & io )
{
   Expr * a = arg->cons.car;
   Expr * b = arg->cons.cdr->cons.car;

   bool is_lt = false;
   if( ( a->type == b->type ) && a->is_atom() )
   {
      is_lt = ( b->atom ) > ( a->atom );
   }
   else
   {
      is_lt = false;
   }

   return make_boolean( is_lt );
}

///////////////////////////////////////////////////////////////////////////////

Expr * f_le( Expr * arg, Context & context, const IO & io )
{

   Expr * a = arg->cons.car;
   Expr * b = arg->cons.cdr->cons.car;

   if( ( a->type != b->type ) && a->is_atom() )
   {
      return make_error( "atom expected" );
   }

   return make_boolean( ( a->atom == b->atom ) || ( b->atom > a->atom ) );
}

///////////////////////////////////////////////////////////////////////////////

Expr * f_eq( Expr * arg, Context & context, const IO & io )
{
   Expr * current = arg;

   bool is_eq = true;
   while( ( is_eq ) && ( current->is_cons() ) && ( current->cons.car->is_atom() ) && ( !current->cons.cdr->is_nil() ) )
   {
      is_eq   = ( current->cons.car->atom == current->cons.cdr->cons.car->atom );
      current = current->cons.cdr;
   }

   return make_boolean( is_eq );
}

///////////////////////////////////////////////////////////////////////////////

Expr * f_not( Expr * arg, Context & context, const IO & io )
{
   assert( arg->is_cons() );
   return make_boolean( !( arg->cons.car->is_truthy() ) );
}

///////////////////////////////////////////////////////////////////////////////

Expr * f_is_null( Expr * arg, Context & context, const IO & io )
{
   assert( arg->is_cons() );
   return make_boolean( arg->cons.car->is_nil() );
}

///////////////////////////////////////////////////////////////////////////////

Expr * f_is_number( Expr * arg, Context & context, const IO & io )
{
   assert( arg->is_cons() );
   return make_boolean( arg->cons.car->is_number() );
}

///////////////////////////////////////////////////////////////////////////////

Expr * f_is_string( Expr * arg, Context & context, const IO & io )
{
   assert( arg->is_cons() );
   return make_boolean( arg->cons.car->is_string() );
}

///////////////////////////////////////////////////////////////////////////////

Expr * f_eval( Expr * arg, Context & context, const IO & io )
{
   if( !arg->is_cons() )
   {
      return make_error( "eval expects a cons" );
   }
   return eval( arg->cons.car, context, io );
}

///////////////////////////////////////////////////////////////////////////////

Expr * f_read( Expr * arg, Context & context, const IO & io )
{
   if( !( arg->is_cons() && arg->cons.car->is_string() ) )
   {
      return make_error( "read expects a string" );
   }
   const char * str = arg->cons.car->atom.string;
   Expr * expr      = parse( tokenize( std::string( str ) ) );
   return expr;
}

///////////////////////////////////////////////////////////////////////////////

Expr * f_read_file( Expr * arg, Context & context, const IO & io )
{
   if( !( arg->is_cons() && arg->cons.car->is_string() ) )
   {
      return make_error( "read-file expects a string" );
   }

   const char * filename = arg->cons.car->atom.string;
   std::ifstream file( filename );
   if( !file )
   {
      return make_error( "could not open file" );
   }
   else
   {
      std::ostringstream ss;
      ss << file.rdbuf();
      std::string content = ss.str();
      return make_string( content.c_str() );
   }
}

Expr * f_exit( Expr * arg, Context & context, const IO & io )
{
   context.exit = true;

   if( arg->cons.car->is_number() )
   {
      context.exit_code = ( int ) arg->cons.car->atom.number;
   }
   else
   {
      context.exit_code = 0;
   }

   return make_void();
}

Expr * f_list( Expr * arg, Context & context, const IO & io )
{
   return arg;
}

///////////////////////////////////////////////////////////////////////////////

} // namespace builtin

} // namespace lisp