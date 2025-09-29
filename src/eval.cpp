#include "eval.h"
#include "builtin.h"
#include "expr.h"
#include "gc.h"
#include "logger.h"
#include "parser.h"
#include "tokenizer.h"

#include <cstdio>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

#ifdef __linux__
#include <readline/history.h>
#include <readline/readline.h>
#endif

namespace lisp
{

///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////

Context::Context( Context * parent )
    : gc::Garbage()
    , m_parent( parent )
    , exit( false )
    , exit_code( false )
{
   if( is_root() )
   {
      load_runtime();
#if 0
      load_stdlib();
#endif
   }
}

///////////////////////////////////////////////////////////////////////////////

Context::~Context()
{
   m_env.clear();
#if 0
   if( m_parent == nullptr )
   {
      gc::run( *this );
   }
#endif

#if 1
   if( is_root() )
   {
      gc::delete_all();
   }
#endif
}

///////////////////////////////////////////////////////////////////////////////

Expr * Context::lookup( const char * symbol ) const
{
   std::string key( symbol );
   auto it = m_env.find( key );
   if( it != m_env.end() )
   {
      return it->second;
   }
   else
   {
      if( m_parent != nullptr )
      {
         return m_parent->lookup( symbol );
      }
      else
      {
         std::string msg = "undefined symbol '" + std::string( symbol ) + "'";
         return make_error( msg.c_str() );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void Context::define( const char * symbol, Expr * expr )
{
   std::string key( symbol );
   m_env[key] = expr;
}

///////////////////////////////////////////////////////////////////////////////

void Context::print( const IO & io ) const
{
   for( auto it = m_env.begin(); it != m_env.end(); it++ )
   {
      io.out << it->first << " : " << to_string_repr( it->second ) << std::endl;
   }
}

const Env & Context::env() const
{
   return m_env;
}

void Context::mark()
{
   set_marked( true );
#if 0
   for( const auto & [_, expr] : m_env )
   {
      expr->mark();
   }
#endif
}

///////////////////////////////////////////////////////////////////////////////

void Context::load_stdlib()
{
   IO io;
   const char * stdlib = R"(
(define euler 2.7)

(define pi 3.14159265359)

(define import
  (lambda (filename)
    (eval
      (read
        (read-file filename)))))
   )";
   int r               = eval( stdlib, *this, io );
   assert( r == 0 );
}

bool Context::is_root() const
{
   return m_parent == nullptr;
}

///////////////////////////////////////////////////////////////////////////////

void Context::load_runtime()
{
   define( "+", make_native( builtin::f_add ) );
   define( "-", make_native( builtin::f_sub ) );
   define( "*", make_native( builtin::f_mul ) );
   define( "/", make_native( builtin::f_div ) );

   define( "=", make_native( builtin::f_eq ) );
   define( ">", make_native( builtin::f_gt ) );
   define( ">=", make_native( builtin::f_ge ) );
   define( "<", make_native( builtin::f_lt ) );
   define( "<=", make_native( builtin::f_le ) );
   define( "not", make_native( builtin::f_not ) );

   define( KW_NIL, make_symbol( KW_NIL ) );
   define( KW_IF, make_symbol( KW_IF ) );
   define( KW_LAMBDA, make_symbol( KW_LAMBDA ) );
   define( KW_DEFINE, make_symbol( KW_DEFINE ) );
   define( KW_QUOTE, make_symbol( KW_QUOTE ) );
   define( KW_PROGN, make_symbol( KW_PROGN ) );
   define( KW_DEFUN, make_symbol( KW_DEFUN ) );

   define( "str", make_native( builtin::f_str ) );
   define( "print", make_native( builtin::f_print ) );
   define( "println", make_native( builtin::f_println ) );
   define( "to-json", make_native( builtin::f_to_json ) );

   define( KW_CAR, make_native( builtin::f_car ) );
   define( KW_CDR, make_native( builtin::f_cdr ) );
   define( KW_CONS, make_native( builtin::f_cons ) );
   define( KW_APPEND, make_native( builtin::f_append ) );
   define( "list", make_native( builtin::f_list ) );
   define( "length", make_native( builtin::f_length ) );

   define( "read", make_native( builtin::f_read ) );
   define( "eval", make_native( builtin::f_eval ) );

   define( "read-file", make_native( builtin::f_read_file ) );

   define( "exit", make_native( builtin::f_exit ) );
   define( "error", make_native( builtin::f_error ) );

   define( "null?", make_native( builtin::f_is_null ) );
   define( "number?", make_native( builtin::f_is_number ) );
   define( "string?", make_native( builtin::f_is_string ) );
   define( "error?", make_native( builtin::f_is_error ) );

   define( "map", make_native( builtin::f_map ) );
   define( "filter", make_native( builtin::f_filter ) );
   define( "load", make_native( builtin::f_load ) );
}

///////////////////////////////////////////////////////////////////////////////

Expr * eval_atom( Expr * expr, Context & context, const IO & io )
{
   assert( expr->is_atom() );
   switch( expr->atom.type )
   {
      case Atom::ATOM_NIL :
      case Atom::ATOM_BOOLEAN :
      case Atom::ATOM_NUMBER :
      case Atom::ATOM_STRING :

      case Atom::ATOM_ERROR :
      case Atom::ATOM_LAMBDA :
      case Atom::ATOM_NATIVE :
         return expr;
      case Atom::ATOM_SYMBOL :
         return context.lookup( expr->atom.symbol );
      default :
         assert( false && "unreachable" );
         return make_nil();
   }
}

///////////////////////////////////////////////////////////////////////////////

Expr * eval_program( Expr * program, Context & context, const IO & io )
{
   Expr * result = make_nil();
   for( ; program->is_cons(); program = program->cdr() )
   {
      Expr * expr = program->car();
      log() << "in: " << expr->to_json() << std::endl;
      result = eval( expr, context, io );
      log() << "out: " << result->to_json() << std::endl;
   }
   return result;
}

///////////////////////////////////////////////////////////////////////////////

Expr * eval_list( Expr * expr, Context & context, const IO & io )
{
   if( expr->type == Expr::EXPR_VOID )
   {
      return make_void();
   }

   if( ( expr->type == Expr::EXPR_ATOM && expr->atom.type == Atom::ATOM_NIL ) )
   {
      return make_nil();
   }

   Expr * first = eval( expr->car(), context, io );
   Expr * rest  = eval_list( expr->cdr(), context, io );
   return make_cons( first, rest );
}

///////////////////////////////////////////////////////////////////////////////

void bind_params( Context * local, Expr * params, Expr * args )
{
   Expr *arg, *param;
   for( arg = args, param = params; param->is_cons() && arg->is_cons(); arg = arg->cdr(), param = param->cdr() )
   {
      const char * symbol = param->car()->as_symbol();

      if( symbol[0] == '&' )
      {
         local->define( symbol + 1, arg );
         break;
      }

      local->define( symbol, arg->car() );
   }
}

///////////////////////////////////////////////////////////////////////////////

Expr * expand( Expr * ast )
{
   if( ast->is_cons() )
   {
      Expr * car = ast->car();
      Expr * cdr = ast->cdr();

      log() << __FUNCTION__ << std::endl;
      log() << "CAR: " << car->to_json() << std::endl;
      log() << "CDR: " << cdr->to_json() << std::endl;

      if( car->is_cons() && car->car()->is_symbol( KW_UNQUOTE ) )
      {
         Expr * sy       = make_symbol( KW_CONS );
         Expr * unquoted = car->cdr()->car();
         log() << "UNQUOTED: " << unquoted->to_json() << std::endl;
         Expr * rest = expand( cdr );
         log() << "REST: " << rest->to_json() << std::endl;
         return make_list( sy, unquoted, rest );
      }

      if( car->is_cons() && car->car()->is_symbol( KW_UNQUOTE_SPLICE ) )
      {
         Expr * sy       = make_symbol( KW_APPEND );
         Expr * unquoted = car->cdr()->car();
         log() << "UNQUOTED: " << unquoted->to_json() << std::endl;
         Expr * rest = expand( cdr );
         log() << "REST: " << rest->to_json() << std::endl;
         return make_list( sy, unquoted, rest );
      }

      Expr * sy    = make_symbol( KW_CONS );
      Expr * first = expand( car );
      log() << "FIRST:" << first->to_json() << std::endl;

      Expr * rest = expand( cdr );
      log() << "REST: " << rest->to_json() << std::endl;
      return make_list( sy, first, rest );
   }
   else if( ast->is_nil() )
   {
      // if ast is nil, return nil
      return make_nil();
   }
   else
   {
      // if ast is atom, return quoted atom
      log() << ast->to_json() << std::endl;
      return make_list( make_symbol( KW_QUOTE ), ast );
   }
}

///////////////////////////////////////////////////////////////////////////////

Expr * eval( Expr * expr, Context & _context, const IO & io )
{
   Context * context = &( _context );
   while( true )
   {
      switch( expr->type )
      {
         case Expr::EXPR_ATOM :
            {
               log() << "ATOM: " << expr->to_json() << std::endl;
               return eval_atom( expr, *context, io );
            }
         case Expr::EXPR_CONS :
            {
               Expr * op   = expr->car();
               Expr * args = expr->cdr();

               log() << "OP  : " << op->to_json() << std::endl;
               log() << "ARG : " << args->to_json() << std::endl;

               if( op->is_symbol( KW_QUOTE ) )
               {
                  return args->car();
               }
               else if( op->is_symbol( KW_UNQUOTE ) )
               {
                  assert( false && "eval unquote is unreachable" );
                  log( LL_WARNING ) << "is this correct? Or should this be replaced in the expansion?" << std::endl;
                  return args->car();
               }
               else if( op->is_symbol( KW_QUASIQUOTE ) )
               {
                  Expr * a = args->car();
                  expr     = expand( a );
                  log() << "QUASIQUOTE : " << expr->to_json() << std::endl;
                  continue;
               }
               else if( op->is_symbol( KW_DEFINE ) )
               {
                  Expr * var   = args->car();
                  Expr * value = eval( args->cdr()->car(), *context, io );
                  context->define( var->atom.symbol, value );
                  return make_void();
               }
               else if( op->is_symbol( KW_LAMBDA ) )
               {
                  Expr * params = args->car();
                  Expr * body   = args->cdr();
                  return make_lambda( params, body, context );
               }
               else if( op->is_symbol( KW_IF ) )
               {
                  Expr * cond      = eval( args->car(), *context, io );
                  Expr * then_expr = args->cdr()->car();
                  Expr * else_expr = args->cdr()->cdr()->car();
                  expr             = ( cond->is_truthy() ) ? then_expr : else_expr;
                  continue;
               }
               else if( op->is_symbol( KW_PROGN ) )
               {
                  for( Expr * it = args; it->is_cons(); it = it->cdr() )
                  {
                     Expr * car = it->car();
                     Expr * cdr = it->cdr();
                     if( cdr->is_nil() )
                     {
                        expr = car;
                     }
                     else
                     {
                        ( void ) eval( car, *context, io );
                     }
                  }
                  continue;
               }
               else if( op->is_symbol( KW_LET ) )
               {
                  Expr * bindings = args->car();
                  Expr * body     = args->cdr()->car();
                  Context * local = gc::alloc<Context>( context );
                  for( Expr * it = bindings; it->is_cons(); it = it->cdr() )
                  {
                     Expr * binding = it->car();
                     Expr * symbol  = binding->car();
                     Expr * value   = binding->cdr()->car();
                     value          = eval( value, *local, io );
                     local->define( symbol->as_symbol(), value );
                  }

                  context = local;
                  expr    = body;
                  continue;
               }
               else if( op->is_symbol( KW_COND ) )
               {
                  Expr * body = nullptr;
                  for( Expr * it = args; it->is_cons(); it = it->cdr() )
                  {
                     Expr * cond   = it->car()->car();
                     Expr * result = eval( cond, *context, io );
                     if( result->is_truthy() )
                     {
                        body = it->car()->cdr();
                        break;
                     }
                  }

                  if( body == nullptr )
                  {
                     body = make_error( "cond expects at least one true condition" );
                  }

                  expr = body;
                  continue;
               }
               else if( op->is_symbol( KW_MACRO ) )
               {
                  Expr * params = args->car();
                  Expr * body   = args->cdr();
                  return make_macro( params, body, context );
               }
               else
               {
                  Expr * fn = eval( op, *context, io );
                  log() << "FN  : " << fn->to_json() << std::endl;
                  if( fn->is_macro() )
                  {
                     Context * new_env = gc::alloc<Context>( fn->atom.macro.env );
                     bind_params( new_env, fn->atom.macro.params, args );

                     expr    = fn->atom.macro.body->car();
                     context = new_env;

                     log() << "before expansion: " << expr->to_json() << std::endl;

                     expr = eval( expr, *context, io );

                     log() << "after expansion: " << expr->to_json() << std::endl;
                     continue;
                  }
                  else
                  {
                     args = eval_list( args, *context, io );
                     log() << "ARGS: " << args->to_json() << std::endl;

                     if( fn->is_native() )
                     {
                        return fn->atom.native( args, *context, io );
                     }
                     else if( fn->is_lambda() && !args->is_nil() )
                     {
                        Context * new_env = gc::alloc<Context>( fn->atom.lambda.env );
                        bind_params( new_env, fn->atom.lambda.params, args );

                        expr    = fn->atom.lambda.body->car();
                        context = new_env;
                        continue;
                     }
                     else
                     {
                        return fn;
                     }
                  }
               }
            }
         default :
            return make_void();
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void print_debug( std::ostream & os, const Tokens & tokens )
{
   for( const Token & tkn : tokens )
   {
      os << "'" << tkn.lexeme << "'"
         << ", ";
   }
   os << std::endl;
}

///////////////////////////////////////////////////////////////////////////////

int eval( const std::string & source, Context & context, const IO & io, Flags flags )
{
   Tokens tokens = tokenize( source );
   if( tokens.empty() )
   {
      return 1;
   }

   if( flags & FLAG_DUMP_TOKENS )
   {
      io.out << "---begin-tokens---" << std::endl;
      print_debug( io.out, tokens );
      io.out << "----end-tokens----" << std::endl;
   }

   Expr * program = parse( tokens );
   if( !program )
   {
      return 2;
   }

   if( flags & FLAG_DUMP_AST )
   {
      io.out << "---begin-program---" << std::endl;
      io.out << program->to_json() << std::endl;
      io.out << "----end-program----" << std::endl;
   }

   Expr * res = eval_program( program, context, io );
   if( !res )
   {
      return 3;
   }

   if( flags & FLAG_DUMP_ENV )
   {
      io.out << "---begin-env---" << std::endl;
      context.print( io );
      io.out << "----env-env----" << std::endl;
   }

   if( res->is_error() )
   {
      io.err << to_string_repr( res );
   }
   else if( ( flags & FLAG_INTERACTIVE ) && !res->is_void() )
   {
      io.out << to_string_repr( res );
      if( flags & FLAG_NEWLINE )
         io.out << std::endl;
   }

   return 0;
}

///////////////////////////////////////////////////////////////////////////////

int eval( const std::string & source, Flags flags )
{
   IO io;
   Context ctx;
   return eval( source, ctx, io, flags );
}

///////////////////////////////////////////////////////////////////////////////

const std::string DBG_CMD = "dbg";

///////////////////////////////////////////////////////////////////////////////

void print_compiler_info()
{
   std::cout << "Compiled on " << __DATE__ << " at " << __TIME__ << " with ";
#if defined( __GNUC__ )
   std::cout << "GCC " << __GNUC__ << "." << __GNUC_MINOR__ << "." << __GNUC_PATCHLEVEL__;
#elif defined( __clang__ )
   std::cout << "Clang " << __clang_major__ << "." << __clang_minor__ << "." << __clang_patchlevel__;
#elif defined( _MSC_VER )
   std::cout << "MSVC " << _MSC_VER;
#else
   std::cout << "(Unknown Compiler)";
#endif
   std::cout << " for ";
#if defined( __linux__ )
   std::cout << "Linux" << std::endl;
#elif defined( _WIN32 )
   std::cout << "Windows" << std::endl;
#elif defined( __APPLE__ )
   std::cout << "macOS" << std::endl;
#else
   std::cout << "(Unknown OS)" << std::endl;
#endif
}

///////////////////////////////////////////////////////////////////////////////

void print_repl_header()
{
   std::cout << "Welcome to my LISP Interpreter!" << std::endl;
   print_compiler_info();
   std::cout << "Copyright (C) 2025 Jakob Maier <jakob.g.maier@gmail.com>" << std::endl;
   std::cout << std::endl;
   std::cout << "Type (exit) to quit." << std::endl;
   std::cout << std::endl;
}

///////////////////////////////////////////////////////////////////////////////

bool read_line( const char * prompt, std::string & line )
{
#if defined( __linux__ ) && 0
   char * input;
   if( ( input = readline( prompt ) ) == NULL )
      return false;

   if( *input )
      add_history( input );

   line = std::string( input );
   free( input );
   return true;
#else
   std::cout << prompt;
   if( !std::getline( std::cin, line ) )
      return false;

   return true;
#endif
}

///////////////////////////////////////////////////////////////////////////////

int repl()
{
   IO io( std::cout, std::cerr );
   Context ctx;
   int res             = 0;
   const char * prompt = "> ";

   print_repl_header();

   do
   {
      std::string line;
      if( !read_line( prompt, line ) )
         break;

      if( line.empty() )
         continue;

      res = eval( line, ctx, io, FLAG_NEWLINE | FLAG_INTERACTIVE );

   } while( ( res == 0 ) && ( !ctx.exit ) );

   return ( ctx.exit_code == 0 ) ? res : ctx.exit_code;
}

} // namespace lisp
