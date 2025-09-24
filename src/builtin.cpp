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

Expr * f_print( Expr * arg, Context & context, const IO & io )
{
   if( !arg->is_cons() )
   {
      return make_error( "invalid-type" );
   }
   Expr * expr     = arg->car();
   std::string str = to_string( expr );
   io.out << str;
   return make_void();
}

Expr * f_println( Expr * arg, Context & context, const IO & io )
{
   ( void ) f_print( arg, context, io );
   io.out << std::endl;
   return make_void();
}

Expr * f_to_json( Expr * arg, Context & context, const IO & io )
{
   return make_string( arg->car()->to_json().c_str() );
}

///////////////////////////////////////////////////////////////////////////////

Expr * f_add( Expr * arg, Context & context, const IO & io )
{
   assert( arg->is_cons() );

   if( !arg->car()->is_number() )
   {
      return make_error( "expected a number" );
   }

   double result = arg->car()->atom.number;
   arg           = arg->cdr();

   while( ( arg->is_cons() ) )
   {
      if( !arg->car()->is_number() )
      {
         return make_error( "expected a number" );
      }
      result += arg->car()->atom.number;
      arg = arg->cdr();
   }

   return make_number( result );
}

///////////////////////////////////////////////////////////////////////////////

Expr * f_sub( Expr * arg, Context & context, const IO & io )
{
   assert( arg->is_cons() );

   if( !arg->car()->is_number() )
   {
      return make_error( "expected a number" );
   }

   double result = arg->car()->atom.number;
   arg           = arg->cdr();

   while( ( arg->is_cons() ) )
   {
      if( !arg->car()->is_number() )
      {
         return make_error( "expected a number" );
      }
      result -= arg->car()->atom.number;
      arg = arg->cdr();
   }

   return make_number( result );
}

///////////////////////////////////////////////////////////////////////////////

Expr * f_mul( Expr * arg, Context & context, const IO & io )
{
   assert( arg->is_cons() );

   if( !arg->car()->is_number() )
   {
      return make_error( "expected a number" );
   }

   double result = arg->car()->atom.number;
   arg           = arg->cdr();

   while( ( arg->is_cons() ) )
   {
      if( !arg->car()->is_number() )
      {
         return make_error( "expected a number" );
      }
      result *= arg->car()->atom.number;
      arg = arg->cdr();
   }

   return make_number( result );
}

///////////////////////////////////////////////////////////////////////////////

Expr * f_div( Expr * arg, Context & context, const IO & io )
{
   assert( arg->is_cons() );

   if( !arg->car()->is_number() )
   {
      return make_error( "expected a number" );
   }

   double result = arg->car()->atom.number;
   arg           = arg->cdr();

   while( ( arg->is_cons() ) )
   {
      if( !arg->car()->is_number() )
      {
         return make_error( "expected a number" );
      }

      if( arg->car()->atom.number == 0 )
      {
         return make_error( "division by zero" );
      }
      result /= arg->car()->atom.number;
      arg = arg->cdr();
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
   return arg->car()->car();
}

///////////////////////////////////////////////////////////////////////////////

Expr * f_cdr( Expr * arg, Context & context, const IO & io )
{
   if( !arg->is_cons() )
   {
      make_error( "car expected 1 argument" );
   }
   return arg->car()->cdr();
}

///////////////////////////////////////////////////////////////////////////////

Expr * f_cons( Expr * arg, Context & context, const IO & io )
{
   Expr * a = arg->car();
   Expr * b = arg->cdr()->car();
   return make_cons( a, b );
}

///////////////////////////////////////////////////////////////////////////////

Expr * f_gt( Expr * arg, Context & context, const IO & io )
{
   Expr * a = arg->car();
   Expr * b = arg->cdr()->car();

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
   Expr * a = arg->car();
   Expr * b = arg->cdr()->car();

   if( ( a->type != b->type ) && a->is_atom() )
   {
      return make_error( "atom expected" );
   }

   return make_boolean( ( a->atom == b->atom ) || ( a->atom > b->atom ) );
}

///////////////////////////////////////////////////////////////////////////////

Expr * f_lt( Expr * arg, Context & context, const IO & io )
{
   Expr * a = arg->car();
   Expr * b = arg->cdr()->car();

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

   Expr * a = arg->car();
   Expr * b = arg->cdr()->car();

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
   while( ( is_eq ) && ( current->is_cons() ) && ( current->car()->is_atom() ) && ( !current->cdr()->is_nil() ) )
   {
      is_eq   = ( current->car()->atom == current->cdr()->car()->atom );
      current = current->cdr();
   }

   return make_boolean( is_eq );
}

///////////////////////////////////////////////////////////////////////////////

Expr * f_not( Expr * arg, Context & context, const IO & io )
{
   assert( arg->is_cons() );
   return make_boolean( !( arg->car()->is_truthy() ) );
}

///////////////////////////////////////////////////////////////////////////////

Expr * f_is_null( Expr * arg, Context & context, const IO & io )
{
   assert( arg->is_cons() );
   return make_boolean( arg->car()->is_nil() );
}

///////////////////////////////////////////////////////////////////////////////

Expr * f_is_number( Expr * arg, Context & context, const IO & io )
{
   assert( arg->is_cons() );
   return make_boolean( arg->car()->is_number() );
}

///////////////////////////////////////////////////////////////////////////////

Expr * f_is_string( Expr * arg, Context & context, const IO & io )
{
   assert( arg->is_cons() );
   return make_boolean( arg->car()->is_string() );
}

///////////////////////////////////////////////////////////////////////////////

Expr * f_eval( Expr * arg, Context & context, const IO & io )
{
   if( !arg->is_cons() )
   {
      return make_error( "eval expects a cons" );
   }
   return eval( arg->car(), context, io );
}

///////////////////////////////////////////////////////////////////////////////

Expr * f_read( Expr * arg, Context & context, const IO & io )
{
   if( !( arg->is_cons() && arg->car()->is_string() ) )
   {
      return make_error( "read expects a string" );
   }
   const char * str = arg->car()->atom.string;
   Expr * expr      = parse( tokenize( std::string( str ) ) );
   return expr;
}

///////////////////////////////////////////////////////////////////////////////

Expr * f_read_file( Expr * arg, Context & context, const IO & io )
{
   if( !( arg->is_cons() && arg->car()->is_string() ) )
   {
      return make_error( "read-file expects a string" );
   }

   const char * filename = arg->car()->atom.string;
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

   if( arg->is_cons() && arg->car()->is_number() )
   {
      context.exit_code = ( int ) arg->car()->atom.number;
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

Expr * f_append( Expr * args, Context & context, const IO & io )
{
   Expr * arg1 = args->car();
   Expr * arg2 = args->cdr()->car();

   if( arg1->is_nil() )
   {
      return arg2;
   }

   Expr * it = nullptr;

   for( it = arg1; it->cdr()->is_cons(); it = it->cdr() )
   {
   }

   it->cons.cdr = arg2;

   return arg1;
}

Expr * f_length( Expr * args, Context & context, const IO & io )
{
   Expr * arg1 = args->car();

   if( arg1->is_nil() )
   {
      return make_number( 0 );
   }

   if( !arg1->is_cons() )
   {
      return make_error( "length exprected a list" );
   }

   int length = 0;

   for( Expr * it = arg1; it->is_cons(); it = it->cdr() )
   {
      length++;
   }

   return make_number( length );
}

///////////////////////////////////////////////////////////////////////////////

} // namespace builtin

} // namespace lisp