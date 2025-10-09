#include "eval.h"
#include "builtin.h"
#include "expr.h"
#include "gc.h"
#include "util.h"
#include "version.h"
#ifdef __linux__
#include "shell.h"
#endif
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
    , exit( false )
    , exit_code( false )
    , m_parent( parent )
{
  if( is_root() )
  {
    builtin::load( *this );
#if defined( __linux__ )
    shell::load( *this );
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

void Context::defvar( const char * symbol, Expr * expr )
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

bool Context::is_root() const
{
  return m_parent == nullptr;
}

///////////////////////////////////////////////////////////////////////////////

void load_shell_macros( Context & context, const IO & io )
{
  const std::string shell_macros = R"(
; test if a symbol is defined
(defmacro symbolp (s)
  `(let ((e ,s)) (not (error? e))))

; if symbol is defined, return the symbol value, otherwise return the symbol name as a string
(defmacro sh-arg (x)
  `(if (symbolp ,x) ,x (symbol-name x)))

; execute shell command, no quotes needed
; usage:
; (sh ls -la)
; (let ((my-file "my_file.txt")) (sh touch my-file))
(defmacro sh (&rest args)
  `(apply exec (map sh-arg args)))
  )";

  ( void ) eval( shell_macros, context, io );
}

///////////////////////////////////////////////////////////////////////////////

void eval_profile( Context & context, const IO & io )
{
  const char * home = getenv( "HOME" );
  assert( home != nullptr );

  std::string path = std::string( home ) + "/.profile.lsp";

  std::ifstream file( path );
  if( !file.is_open() )
  {
    return;
  }

  std::ostringstream ss;
  ss << file.rdbuf();
  std::string program = ss.str();

  ( void ) eval( program, context, io );
}

///////////////////////////////////////////////////////////////////////////////

void init( Context & context, const IO & io )
{
  load_shell_macros( context, io );
  eval_profile( context, io );
}

///////////////////////////////////////////////////////////////////////////////

Expr * eval_atom( Expr * expr, Context & context, const IO & io )
{
  assert( expr->is_atom() );
  switch( expr->atom.type )
  {
    case Atom::ATOM_NIL :
    case Atom::ATOM_BOOLEAN :
    case Atom::ATOM_REAL :
    case Atom::ATOM_INTEGER :
    case Atom::ATOM_STRING :
    case Atom::ATOM_ERROR :
    case Atom::ATOM_LAMBDA :
    case Atom::ATOM_MACRO :
    case Atom::ATOM_NATIVE :
      return expr;
    case Atom::ATOM_SYMBOL :
      return context.lookup( expr->atom.symbol );
  }
  UNREACHABLE
  return make_nil();
}

///////////////////////////////////////////////////////////////////////////////

Expr * eval_program( Expr * program, Context & context, const IO & io )
{
  Expr * result = make_nil();
  for( ; program->is_cons(); program = program->cdr() )
  {
    Expr * expr = program->car();
    result      = eval( expr, context, io );
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
    assert( symbol != nullptr );

    if( strncmp( symbol, "&rest", 5 ) == 0 )
    {
      const char * next_symbol = param->cdr()->car()->as_symbol();
      assert( next_symbol != nullptr );
      local->defvar( next_symbol, arg );
      break;
    }

    local->defvar( symbol, arg->car() );
  }
}

///////////////////////////////////////////////////////////////////////////////

Expr * expand( Expr * ast )
{
  if( ast->is_cons() )
  {
    Expr * car = ast->car();
    Expr * cdr = ast->cdr();

    if( car->is_cons() && car->car()->is_symbol( KW_UNQUOTE ) )
    {
      Expr * sy       = make_symbol( KW_CONS );
      Expr * unquoted = car->cdr()->car();
      Expr * rest     = expand( cdr );
      return make_list( sy, unquoted, rest );
    }

    if( car->is_cons() && car->car()->is_symbol( KW_UNQUOTE_SPLICE ) )
    {
      Expr * sy       = make_symbol( KW_APPEND );
      Expr * unquoted = car->cdr()->car();
      Expr * rest     = expand( cdr );
      return make_list( sy, unquoted, rest );
    }

    Expr * sy    = make_symbol( KW_CONS );
    Expr * first = expand( car );
    Expr * rest  = expand( cdr );
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
          return eval_atom( expr, *context, io );
        }
      case Expr::EXPR_CONS :
        {
          Expr * op   = expr->car();
          Expr * args = expr->cdr();

          if( op->is_symbol( KW_QUOTE ) )
          {
            return args->car();
          }
          else if( op->is_symbol( KW_UNQUOTE ) )
          {
            UNREACHABLE
            return args->car();
          }
          else if( op->is_symbol( KW_QUASIQUOTE ) )
          {
            Expr * a = args->car();
            expr     = expand( a );
            continue;
          }
          else if( op->is_symbol( KW_DEFINE ) )
          {
            Expr * var   = args->car();
            Expr * value = eval( args->cdr()->car(), *context, io );
            context->defvar( var->atom.symbol, value );
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
              local->defvar( symbol->as_symbol(), value );
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
          else if( op->is_symbol( KW_OR ) )
          {
            for( Expr * it = args; it->is_cons(); it = it->cdr() )
            {
              Expr * res = eval( it->car(), *context, io );
              if( res->is_truthy() )
                return make_boolean( true );
            }
            return make_boolean( false );
          }
          else if( op->is_symbol( KW_AND ) )
          {
            for( Expr * it = args; it->is_cons(); it = it->cdr() )
            {
              Expr * res = eval( it->car(), *context, io );
              if( !res->is_truthy() )
                return make_boolean( false );
            }
            return make_boolean( true );
          }
          else if( op->is_symbol( KW_MACRO ) )
          {
            Expr * params = args->car();
            Expr * body   = args->cdr();
            return make_macro( params, body, context );
          }
#ifdef __linux__
          else if( op->is_symbol( KW_TO_STREAM ) )
          {
            Expr * r         = eval( args, *context, io );
            std::string data = to_string( r );

            ssize_t n;
            ssize_t total = 0;

            while( total < ( ssize_t ) data.size() )
            {
              n = write( io.pipe_stdout, data.data() + total, data.size() - total );
              if( n < 0 )
              {
                break;
              }
              total += n;
            }

            if( io.pipe_stdout != STDOUT_FILENO )
            {
              close( io.pipe_stdout );
            }
          }
          else if( op->is_symbol( KW_FROM_STREAM ) )
          {
            int fds[2];
            pipe( fds );

            IO local_io;
            local_io.pipe_stdin  = io.pipe_stdin;
            local_io.pipe_stdout = fds[1];

            Expr * r = eval( args, *context, local_io );
            if( r->is_error() )
            {
              return r;
            }

            char tmp[1024];
            ssize_t total = 0;
            char * buffer = nullptr;

            while( true )
            {
              ssize_t bytes_read = read( fds[0], tmp, sizeof( tmp ) );
              if( bytes_read <= 0 )
              {
                break;
              }

              buffer = ( char * ) realloc( buffer, total + bytes_read + 1 );
              memcpy( buffer + total, tmp, bytes_read );
              total += bytes_read;
            }

            if( total > 0 )
            {
              buffer[total] = '\0';
              // remove trailing newline
              if( buffer[total - 1] == '\n' )
              {
                buffer[total - 1] = '\0';
              }
            }

            close( fds[0] );
            close( fds[1] );

            return make_string_take_ownership( buffer );
          }
          else if( op->is_symbol( KW_PIPE ) )
          {
            Expr * exec1 = args->car();
            Expr * exec2 = args->cdr()->car();

            int fds[2];
            pipe( fds );
            Expr * r;

            {
              Context * local = gc::alloc<Context>( context );
              IO local_io;
              local_io.pipe_stdin  = io.pipe_stdin;
              local_io.pipe_stdout = fds[1];
              ( void ) eval( exec1, *local, local_io );
            }
            {

              Context * local = gc::alloc<Context>( context );
              IO local_io;
              local_io.pipe_stdin  = fds[0];
              local_io.pipe_stdout = io.pipe_stdout;
              r                    = eval( exec2, *local, local_io );
            }

            close( fds[0] );
            close( fds[1] );
            return r;
          }
#endif
          else
          {
            Expr * fn = eval( op, *context, io );
            if( fn->is_macro() )
            {
              // Context * new_env = gc::alloc<Context>( fn->atom.macro.env );
              Context * new_env = gc::alloc<Context>( context );
              bind_params( new_env, fn->atom.macro.params, args );

              expr    = fn->atom.macro.body->car();
              context = new_env;

              expr = eval( expr, *context, io );
              continue;
            }
            else
            {
              args = eval_list( args, *context, io );

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
    os << "'" << tkn.lexeme << "'" << ", ";
  }
  os << std::endl;
}

///////////////////////////////////////////////////////////////////////////////

int eval( const std::string & source, Context & context, const IO & io, Flags flags )
{
  Tokens tokens = tokenize( source );
  if( tokens.empty() )
    return 1;

  Expr * program = parse( tokens );
  if( !program )
    return 2;

  Expr * res = eval_program( program, context, io );
  if( !res )
    return 3;

  if( res->is_error() )
  {
    io.err << to_string_repr( res ) << std::endl;
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

  if( flags & FLAG_INIT )
  {
    init( ctx, io );
  }

  return eval( source, ctx, io, flags );
}

///////////////////////////////////////////////////////////////////////////////

const std::string DBG_CMD = "dbg";

///////////////////////////////////////////////////////////////////////////////

std::string get_current_time()
{
  std::stringstream os;
  os << __DATE__ << ", ";
  os << __TIME__;
  return os.str();
}

///////////////////////////////////////////////////////////////////////////////

std::string get_compiler_info()
{
  std::stringstream os;
#if defined( __clang__ )
  os << "Clang " << __clang_major__ << "." << __clang_minor__ << "." << __clang_patchlevel__;
#elif defined( __GNUC__ )
  os << "GCC " << __GNUC__ << "." << __GNUC_MINOR__ << "." << __GNUC_PATCHLEVEL__;
#elif defined( _MSC_VER )
  os << "MSVC " << _MSC_VER;
#else
  os << "Unknown Compiler";
#endif
  os << ", ";
#if defined( __linux__ )
  os << "Linux";
#elif defined( _WIN32 )
  os << "Windows";
#elif defined( __APPLE__ )
  os << "macOS";
#else
  os << "(Unknown OS)";
#endif
  return os.str();
}

///////////////////////////////////////////////////////////////////////////////

std::string get_copyright_info()
{
  return "Copyright (C) 2025 Jakob Maier <jakob.g.maier@gmail.com>";
}

///////////////////////////////////////////////////////////////////////////////

std::string get_version_info()
{
  return std::string( GIT_HASH );
}

///////////////////////////////////////////////////////////////////////////////

void print_repl_header()
{
  std::cout << "Welcome to Redstart! (";
  std::cout << get_version_info() << ", ";
  std::cout << get_current_time() << ", ";
  std::cout << get_compiler_info() << ")" << std::endl;
  // std::cout << get_copyright_info() << std::endl;
  std::cout << std::endl;
  std::cout << "Type (exit) to quit." << std::endl;
  std::cout << std::endl;
}

///////////////////////////////////////////////////////////////////////////////

void print_version_info()
{
  std::cout << get_version_info() << std::endl;
}

///////////////////////////////////////////////////////////////////////////////

bool read_line( const char * prompt, std::string & line )
{
#ifdef __linux__
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

  init( ctx, io );

  do
  {
    std::string line;
    if( !read_line( prompt, line ) )
      break;

    if( line.empty() )
      continue;

    Expr * form   = parse( line );
    Expr * result = eval( form, ctx, io );

    if( !result->is_error() )
      ctx.defvar( "_", result );

    std::cout << to_string_repr( result ) << std::endl;

  } while( ( res == 0 ) && ( !ctx.exit ) );

  return ( ctx.exit_code == 0 ) ? res : ctx.exit_code;
}

} // namespace lisp
