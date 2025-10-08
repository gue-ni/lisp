#include <cctype>
#include <cstring>
#include <fstream>
#include <sstream>
#include <string>

#include "builtin.h"
#include "eval.h"
#include "expr.h"
#include "parser.h"
#include "tokenizer.h"

namespace lisp
{

///////////////////////////////////////////////////////////////////////////////

int list_length( Expr * lst )
{
  int len = 0;
  for( Expr * it = lst; it->is_cons(); it = it->cdr() )
    len++;
  return len;
}

///////////////////////////////////////////////////////////////////////////////

#define ASSERT_ARG_COUNT( arg_list, n )                                        \
  do                                                                           \
  {                                                                            \
    int len = list_length( arg_list );                                         \
    if( len != n )                                                             \
    {                                                                          \
      std::ostringstream os;                                                   \
      os << __FUNCTION__ << " expected " << n << " args but received " << len; \
      return make_error( os.str().c_str() );                                   \
    }                                                                          \
  } while( 0 )

///////////////////////////////////////////////////////////////////////////////

#define ASSERT_MIN_ARG_COUNT( arg_list, n )                                             \
  do                                                                                    \
  {                                                                                     \
    int len = list_length( arg_list );                                                  \
    if( len < n )                                                                       \
    {                                                                                   \
      std::ostringstream os;                                                            \
      os << __FUNCTION__ << " expected at least " << n << " args but received " << len; \
      return make_error( os.str().c_str() );                                            \
    }                                                                                   \
  } while( 0 )

///////////////////////////////////////////////////////////////////////////////

#define ASSERT_ARG_TYPE( a, atom_type )                                                                              \
  do                                                                                                                 \
  {                                                                                                                  \
    if( !( a )->is_atom( atom_type ) )                                                                               \
    {                                                                                                                \
      std::ostringstream os;                                                                                         \
      os << __FUNCTION__ << " expected " << #a << " to be of type " << atom_type << ", but is " << ( a )->atom.type; \
      return make_error( os.str().c_str() );                                                                         \
    }                                                                                                                \
  } while( 0 )

///////////////////////////////////////////////////////////////////////////////

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

Expr * f_strtok( Expr * args, Context & context, const IO & io )
{
  ASSERT_ARG_COUNT( args, 2 );

  Expr * arg_1 = args->car();
  ASSERT_ARG_TYPE( arg_1, Atom::ATOM_STRING );

  Expr * arg_2 = args->cdr()->car();
  ASSERT_ARG_TYPE( arg_2, Atom::ATOM_STRING );

  const char * delim = arg_1->as_string();
  char * str         = STRDUP( arg_2->as_string() );

  ListBuilder lb;
  for( char * token = strtok( str, delim ); token != NULL; token = strtok( NULL, delim ) )
  {
    lb.append( make_string( token ) );
  }

  free( str );
  return lb.list();
}

///////////////////////////////////////////////////////////////////////////////

Expr * f_strlen( Expr * args, Context & context, const IO & io )
{
  ASSERT_ARG_COUNT( args, 1 );

  Expr * arg1 = args->car();
  ASSERT_ARG_TYPE( arg1, Atom::ATOM_STRING );

  return make_integer( strlen( arg1->as_string() ) );
}

///////////////////////////////////////////////////////////////////////////////

Expr * f_strcmp( Expr * args, Context & context, const IO & io )
{
  ASSERT_MIN_ARG_COUNT( args, 2 );

  Expr * arg_1 = args->car();
  ASSERT_ARG_TYPE( arg_1, Atom::ATOM_STRING );

  const char * str1 = arg_1->as_string();

  for( Expr * it = args->cdr(); it->is_cons(); it = it->cdr() )
  {
    Expr * arg_n = it->car();
    ASSERT_ARG_TYPE( arg_n, Atom::ATOM_STRING );

    const char * str2 = arg_n->as_string();
    if( strcmp( str1, str2 ) != 0 )
    {
      return make_boolean( false );
    }
  }

  return make_boolean( true );
}

///////////////////////////////////////////////////////////////////////////////

Expr * f_strip( Expr * args, Context & context, const IO & io )
{
  ASSERT_ARG_COUNT( args, 1 );

  Expr * arg_1 = args->car();
  ASSERT_ARG_TYPE( arg_1, Atom::ATOM_STRING );

  char * str = STRDUP( arg_1->as_string() );

  char * start = str;
  while( *start && isspace( *start ) )
  {
    start++;
  }

  char * end = start + strlen( start ) - 1;
  while( start < end && isspace( *end ) )
  {
    end--;
  }

  *( end + 1 ) = '\0';

  Expr * stripped = make_string( start );
  free( str );
  return stripped;
}

///////////////////////////////////////////////////////////////////////////////

Expr * f_substr( Expr * args, Context & context, const IO & io )
{
  ASSERT_ARG_COUNT( args, 3 );

  Expr * arg_1 = args->car();
  Expr * arg_2 = args->cdr()->car();
  Expr * arg_3 = args->cdr()->cdr()->car();

  if( !arg_1->is_number() )
    return make_error( "substr expects arg 1 to be a number" );

  if( !arg_2->is_number() )
    return make_error( "substr expects arg 2 to be a number" );

  if( !arg_3->is_string() )
    return make_error( "substr expects arg 3 to be a string" );

  int start  = ( int ) arg_1->as_number();
  int end    = ( int ) arg_2->as_number();
  char * str = STRDUP( arg_3->as_string() );
  int len    = strlen( str );

  if( start < 0 || len < end || start >= end )
    return make_error( "index out of range" );

  str[end] = '\0';
  str += start;

  Expr * result = make_string( str );
  free( str );
  return result;
}

///////////////////////////////////////////////////////////////////////////////

Expr * f_char_at( Expr * args, Context & context, const IO & io )
{
  ASSERT_ARG_COUNT( args, 2 );

  Expr * arg_1 = args->car();
  if( !arg_1->is_number() )
    return make_error( "char-at expects number as first arg" );

  Expr * arg_2 = args->cdr()->car();
  if( !arg_2->is_string() )
    return make_error( "char-at exprects string as second arg" );

  int index = ( int ) arg_1->as_number();

  const char * str = arg_2->as_string();
  int len          = strlen( str );

  if( !( 0 <= index && index < len ) )
    return make_error( "index out-of-bounds" );

  char character[2];
  character[0] = str[index];
  character[1] = '\0';
  return make_string( character );
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

Expr * f_add( Expr * args, Context & context, const IO & io )
{
  ASSERT_MIN_ARG_COUNT( args, 2 );

  Expr * arg_1 = args->car();
  if( !arg_1->is_number() )
  {
    return make_error( "expected a number" );
  }

  Expr * result = make_copy( arg_1 );
  for( Expr * it = args->cdr(); it->is_cons(); it = it->cdr() )
  {
    Expr * arg_n = it->car();

    if( !arg_n->is_number() )
    {
      return make_error( "expected a number" );
    }

    if( result->is_integer() && arg_n->is_integer() )
    {
      result->atom.integer += arg_n->as_integer();
    }
    else
    {
      if( result->is_integer() )
      {
        result = make_real( result->as_number() );
      }
      result->atom.real += arg_n->as_number();
    }
  }

  return result;
}

///////////////////////////////////////////////////////////////////////////////

Expr * f_sub( Expr * args, Context & context, const IO & io )
{
  ASSERT_MIN_ARG_COUNT( args, 2 );

  Expr * arg_1 = args->car();
  if( !arg_1->is_number() )
  {
    return make_error( "expected a number" );
  }

  Expr * result = make_copy( arg_1 );
  for( Expr * it = args->cdr(); it->is_cons(); it = it->cdr() )
  {
    Expr * arg_n = it->car();

    if( !arg_n->is_number() )
    {
      return make_error( "expected a number" );
    }

    if( result->is_integer() && arg_n->is_integer() )
    {
      result->atom.integer -= arg_n->as_integer();
    }
    else
    {
      if( result->is_integer() )
      {
        result = make_real( result->as_number() );
      }
      result->atom.real -= arg_n->as_number();
    }
  }
  return result;
}

///////////////////////////////////////////////////////////////////////////////

Expr * f_mul( Expr * args, Context & context, const IO & io )
{
  ASSERT_MIN_ARG_COUNT( args, 2 );

  Expr * arg_1 = args->car();
  if( !arg_1->is_number() )
  {
    return make_error( "expected a number" );
  }

  Expr * result = make_copy( arg_1 );
  for( Expr * it = args->cdr(); it->is_cons(); it = it->cdr() )
  {
    Expr * arg_n = it->car();

    if( !arg_n->is_number() )
    {
      return make_error( "expected a number" );
    }

    if( result->is_integer() && arg_n->is_integer() )
    {
      result->atom.integer *= arg_n->as_integer();
    }
    else
    {
      if( result->is_integer() )
      {
        result = make_real( result->as_number() );
      }
      result->atom.real *= arg_n->as_number();
    }
  }

  return result;
}

///////////////////////////////////////////////////////////////////////////////

Expr * f_div( Expr * args, Context & context, const IO & io )
{
  ASSERT_MIN_ARG_COUNT( args, 2 );

  Expr * arg_1 = args->car();
  if( !arg_1->is_number() )
  {
    return make_error( "expected a number" );
  }

  Expr * result = make_copy( arg_1 );
  for( Expr * it = args->cdr(); it->is_cons(); it = it->cdr() )
  {
    Expr * arg_n = it->car();

    if( !arg_n->is_number() )
    {
      return make_error( "expected a number" );
    }

    if( arg_n->as_number() == 0.0 )
    {
      return make_error( "division by zero" );
    }

    if( result->is_integer() && arg_n->is_integer() )
    {
      result->atom.integer /= arg_n->as_integer();
    }
    else
    {
      if( result->is_integer() )
      {
        result = make_real( result->as_number() );
      }
      result->atom.real /= arg_n->as_number();
    }
  }
  return result;
}

///////////////////////////////////////////////////////////////////////////////

Expr * f_car( Expr * arg, Context & context, const IO & io )
{
  if( !arg->car()->is_cons() )
  {
    return make_error( "car expected cons argument" );
  }
  return arg->car()->car();
}

///////////////////////////////////////////////////////////////////////////////

Expr * f_cdr( Expr * arg, Context & context, const IO & io )
{
  if( !arg->car()->is_cons() )
  {
    return make_error( "cdr expected cons argument" );
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
  ASSERT_ARG_COUNT( arg, 2 );

  Expr * arg_1 = arg->car();
  Expr * arg_2 = arg->cdr()->car();

  if( !( arg_1->is_number() && arg_2->is_number() ) )
  {
    return make_error( "expected numbers" );
  }

  return make_boolean( arg_1->as_number() > arg_2->as_number() );
}

///////////////////////////////////////////////////////////////////////////////

Expr * f_ge( Expr * arg, Context & context, const IO & io )
{
  ASSERT_ARG_COUNT( arg, 2 );

  Expr * arg_1 = arg->car();
  Expr * arg_2 = arg->cdr()->car();

  if( !( arg_1->is_number() && arg_2->is_number() ) )
  {
    return make_error( "expected numbers" );
  }

  return make_boolean( arg_1->as_number() >= arg_2->as_number() );
}

///////////////////////////////////////////////////////////////////////////////

Expr * f_lt( Expr * arg, Context & context, const IO & io )
{
  ASSERT_ARG_COUNT( arg, 2 );

  Expr * arg_1 = arg->car();
  Expr * arg_2 = arg->cdr()->car();

  if( !( arg_1->is_number() && arg_2->is_number() ) )
  {
    return make_error( "expecting numeric type" );
  }

  return make_boolean( arg_1->as_number() < arg_2->as_number() );
}

///////////////////////////////////////////////////////////////////////////////

Expr * f_le( Expr * arg, Context & context, const IO & io )
{
  Expr * arg_1 = arg->car();
  Expr * arg_2 = arg->cdr()->car();

  if( !( arg_1->is_number() && arg_2->is_number() ) )
  {
    return make_error( "atom expected" );
  }

  return make_boolean( arg_1->as_number() >= arg_2->as_number() );
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

Expr * f_is_real( Expr * arg, Context & context, const IO & io )
{
  assert( arg->is_cons() );
  return make_boolean( arg->car()->is_real() );
}

///////////////////////////////////////////////////////////////////////////////

Expr * f_is_integer( Expr * arg, Context & context, const IO & io )
{
  assert( arg->is_cons() );
  return make_boolean( arg->car()->is_integer() );
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
  if( !file.is_open() )
  {
    std::string msg = "Could not open file '" + std::string( filename ) + "': " + std::string( strerror( errno ) );
    return make_error( msg.c_str() );
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

  if( arg->is_cons() && arg->car()->is_real() )
  {
    context.exit_code = ( int ) arg->car()->atom.real;
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
  const char * message = arg->car()->as_string();
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
    return make_integer( 0 );
  }

  if( !arg1->is_cons() )
  {
    return make_error( "length exprected a list" );
  }

  int length = list_length( arg1 );
  return make_integer( length );
}

///////////////////////////////////////////////////////////////////////////////

Expr * f_filter( Expr * arg, Context & context, const IO & io )
{
  Expr * fn = arg->car();
  if( !fn->is_procedure() )
  {
    return make_error( "filter expects a function as first argument" );
  }

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

  if( !fn->is_procedure() )
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

  // register libraries at root context
  Context * root = &context;
  while( root->parent() )
  {
    root = root->parent();
  }

  return f_eval( make_list( r, make_nil() ), *root, io );
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

void load( Context & ctx )
{
  ctx.defvar( "+", make_native( builtin::f_add ) );
  ctx.defvar( "-", make_native( builtin::f_sub ) );
  ctx.defvar( "*", make_native( builtin::f_mul ) );
  ctx.defvar( "/", make_native( builtin::f_div ) );
  ctx.defvar( "=", make_native( builtin::f_eq ) );
  ctx.defvar( ">", make_native( builtin::f_gt ) );
  ctx.defvar( ">=", make_native( builtin::f_ge ) );
  ctx.defvar( "<", make_native( builtin::f_lt ) );
  ctx.defvar( "<=", make_native( builtin::f_le ) );
  ctx.defvar( "not", make_native( builtin::f_not ) );
  ctx.defvar( KW_NIL, make_symbol( KW_NIL ) );
  ctx.defvar( KW_IF, make_symbol( KW_IF ) );
  ctx.defvar( KW_LAMBDA, make_symbol( KW_LAMBDA ) );
  ctx.defvar( KW_DEFINE, make_symbol( KW_DEFINE ) );
  ctx.defvar( KW_QUOTE, make_symbol( KW_QUOTE ) );
  ctx.defvar( KW_PROGN, make_symbol( KW_PROGN ) );
  ctx.defvar( KW_DEFUN, make_symbol( KW_DEFUN ) );
  ctx.defvar( "str", make_native( builtin::f_str ) );
  ctx.defvar( "strtok", make_native( builtin::f_strtok ) );
  ctx.defvar( "strlen", make_native( builtin::f_strlen ) );
  ctx.defvar( "strcmp", make_native( builtin::f_strcmp ) );
  ctx.defvar( "strcat", make_native( builtin::f_str ) );
  ctx.defvar( "strip", make_native( builtin::f_strip ) );
  ctx.defvar( "split", make_native( builtin::f_strtok ) );
  ctx.defvar( "substr", make_native( builtin::f_substr ) );
  ctx.defvar( "char-at", make_native( builtin::f_char_at ) );
  ctx.defvar( "print", make_native( builtin::f_print ) );
  ctx.defvar( "println", make_native( builtin::f_println ) );
  ctx.defvar( "to-json", make_native( builtin::f_to_json ) );
  ctx.defvar( KW_CAR, make_native( builtin::f_car ) );
  ctx.defvar( KW_CDR, make_native( builtin::f_cdr ) );
  ctx.defvar( KW_CONS, make_native( builtin::f_cons ) );
  ctx.defvar( KW_APPEND, make_native( builtin::f_append ) );
  ctx.defvar( "list", make_native( builtin::f_list ) );
  ctx.defvar( "length", make_native( builtin::f_length ) );
  ctx.defvar( "read", make_native( builtin::f_read ) );
  ctx.defvar( "eval", make_native( builtin::f_eval ) );
  ctx.defvar( "read-file", make_native( builtin::f_read_file ) );
  ctx.defvar( "exit", make_native( builtin::f_exit ) );
  ctx.defvar( "error", make_native( builtin::f_error ) );
  ctx.defvar( "null?", make_native( builtin::f_is_null ) );
  ctx.defvar( "string?", make_native( builtin::f_is_string ) );
  ctx.defvar( "error?", make_native( builtin::f_is_error ) );
  ctx.defvar( "symbol?", make_native( builtin::f_is_symbol ) );
  ctx.defvar( "real?", make_native( builtin::f_is_real ) );
  ctx.defvar( "int?", make_native( builtin::f_is_integer ) );
  ctx.defvar( "number?", make_native( builtin::f_is_number ) );
  ctx.defvar( "symbol-name", make_native( builtin::f_symbol_name ) );
  ctx.defvar( "map", make_native( builtin::f_map ) );
  ctx.defvar( "filter", make_native( builtin::f_filter ) );
  ctx.defvar( "apply", make_native( builtin::f_apply ) );
  ctx.defvar( "load", make_native( builtin::f_load ) );
}

///////////////////////////////////////////////////////////////////////////////

} // namespace builtin

} // namespace lisp
