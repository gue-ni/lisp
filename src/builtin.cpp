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

Expr * f_str( Expr * arg, Context & context, const IO & io )
{
   std::string str = "";
   for( Expr * it = arg; it->is_cons(); it = it->cdr() )
   {
      str += to_string( it->car() );
   }
   return make_string( str.c_str() );
}

///////////////////////////////////////////////////////////////////////////////

Expr * f_print( Expr * arg, Context & context, const IO & io )
{
   Expr * str = f_str( arg, context, io );
   io.out << str->atom.string;
   return make_void();
}

///////////////////////////////////////////////////////////////////////////////

Expr * f_println( Expr * arg, Context & context, const IO & io )
{
   ( void ) f_print( arg, context, io );
   io.out << std::endl;
   return make_void();
}

///////////////////////////////////////////////////////////////////////////////

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

Expr * f_is_error( Expr * arg, Context & context, const IO & io )
{
   return make_boolean( arg->car()->is_error() );
}

///////////////////////////////////////////////////////////////////////////////

Expr * f_is_symbol( Expr * arg, Context & context, const IO & io )
{
   assert( arg->is_cons() );
   return make_boolean( arg->car()->is_symbol() );
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

///////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////

Expr * f_error( Expr * arg, Context & context, const IO & io )
{
   const char * message = arg->car()->atom.string;
   return make_error( message );
}

///////////////////////////////////////////////////////////////////////////////

Expr * f_list( Expr * arg, Context & context, const IO & io )
{
   return arg;
}

///////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////

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

Expr * f_filter( Expr * arg, Context & context, const IO & io )
{
   Expr * fn = arg->car();
   assert( fn->is_lambda() || fn->is_native() );

   ListBuilder builder;

   Expr * list = arg->cdr()->car();
   for( Expr * it = list; it->is_cons(); it = it->cdr() )
   {
      Expr * el     = it->car();
      Expr * result = eval( make_list( fn, el ), context, io );

      if( result->is_truthy() )
      {
         builder.append( el );
      }
   }

   return builder.list();
}

///////////////////////////////////////////////////////////////////////////////

Expr * f_map( Expr * arg, Context & context, const IO & io )
{
   Expr * fn = arg->car();

   if( !( fn->is_lambda() || fn->is_native() || fn->is_macro() ) )
   {
      return make_error( "map expects a function as first argument" );
   }

   Expr * list = arg->cdr()->car();

   ListBuilder builder;
   for( Expr * it = list; it->is_cons(); it = it->cdr() )
   {
      Expr * el     = it->car();
      Expr * result = eval( make_list( fn, el ), context, io );
      builder.append( result );
   }

   return builder.list();
}

///////////////////////////////////////////////////////////////////////////////

Expr * f_apply( Expr * arg, Context & context, const IO & io )
{
   Expr * fn = arg->car();
   if( !( fn->is_lambda() || fn->is_native() || fn->is_macro() ) )
   {
      return make_error( "apply expects a function as first argument" );
   }

   Expr * list = arg->cdr()->car();
   Expr * tmp  = make_cons( fn, list );
   return eval( tmp, context, io );
}

///////////////////////////////////////////////////////////////////////////////

Expr * f_load( Expr * arg, Context & context, const IO & io )
{
   Expr * r = f_read_file( arg, context, io );
   if( r->is_error() )
      return r;

   r = f_read( make_list( r, make_nil() ), context, io );
   if( r->is_error() )
      return r;

   return f_eval( make_list( r, make_nil() ), context, io );
}

///////////////////////////////////////////////////////////////////////////////

Expr * f_symbol_name( Expr * arg, Context & context, const IO & io )
{
   Expr * arg1 = arg->car();
   if( !arg1->is_symbol() )
   {
      return make_error( "symbol-name expects a symbol" );
   }
   return make_string( arg1->atom.symbol );
}

///////////////////////////////////////////////////////////////////////////////

} // namespace builtin

} // namespace lisp