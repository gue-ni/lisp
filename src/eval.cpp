#include "eval.h"
#include "builtin.h"
#include "expr.h"
#include "gc.h"
#include "parser.h"
#include "tokenizer.h"

#include <cstdio>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

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
      return ( m_parent != nullptr ) ? ( m_parent->lookup( symbol ) ) : make_nil();
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
      io.out << it->first << " : ";
      it->second->print( io );
      io.out << std::endl;
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
   define( "<", make_native( builtin::f_lt ) );
   define( "not", make_native( builtin::f_not ) );

   define( "print", make_native( builtin::f_print ) );
   define( "println", make_native( builtin::f_println ) );
   define( "print-debug", make_native( builtin::f_print_debug ) );

   define( "car", make_native( builtin::f_car ) );
   define( "cdr", make_native( builtin::f_cdr ) );
   define( "cons", make_native( builtin::f_cons ) );

   define( "list", make_native( builtin::f_list ) );

   define( "read", make_native( builtin::f_read ) );
   define( "eval", make_native( builtin::f_eval ) );

   define( "read-file", make_native( builtin::f_read_file ) );

   define( "exit", make_native( builtin::f_exit ) );
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
         return expr;
      case Atom::ATOM_SYMBOL :
         return context.lookup( expr->atom.symbol );
      case Atom::ATOM_LAMBDA :
      case Atom::ATOM_NATIVE :
      default :
         return make_nil();
   }
}

///////////////////////////////////////////////////////////////////////////////

Expr * eval_program( Expr * program, Context & context, const IO & io )
{
   Expr * result = make_nil();
   while( has_type( program, Expr::EXPR_CONS ) )
   {
      Expr * expr = program->cons.car;
      result      = eval( expr, context, io );
      program     = program->cons.cdr;
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

   Expr * first = eval( expr->cons.car, context, io );
   Expr * rest  = eval_list( expr->cons.cdr, context, io );
   return make_cons( first, rest );
}

///////////////////////////////////////////////////////////////////////////////

void bind_params( Context * local, Expr * params, Expr * args )
{
   Expr * arg   = args;
   Expr * param = params;

   while( param->is_cons() && arg->is_cons() )
   {
      local->define( param->cons.car->atom.symbol, arg->cons.car );
      arg   = arg->cons.cdr;
      param = param->cons.cdr;
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
               Cons cons   = expr->cons;
               Expr * op   = cons.car;
               Expr * args = cons.cdr;

               if( op->is_symbol( "quote" ) )
               {
                  return args->cons.car;
               }
               else if( op->is_symbol( "define" ) )
               {
                  Expr * var   = args->cons.car;
                  Expr * value = eval( args->cons.cdr->cons.car, *context, io );
                  context->define( var->atom.symbol, value );
                  return make_void();
               }
               else if( op->is_symbol( "lambda" ) )
               {
                  Expr * params = args->cons.car;
                  Expr * body   = args->cons.cdr;
                  return make_lambda( params, body, context );
               }
               else if( op->is_symbol( "if" ) )
               {
                  Expr * cond      = eval( args->cons.car, *context, io );
                  Expr * then_expr = args->cons.cdr->cons.car;
                  Expr * else_expr = args->cons.cdr->cons.cdr->cons.car;
                  expr             = ( cond->is_truthy() ) ? then_expr : else_expr;
                  continue;
               }
               else
               {
                  Expr * fn = eval( op, *context, io );
                  args      = eval_list( args, *context, io );

                  if( ( fn->is_atom() ) && !args->is_nil() )
                  {
                     if( fn->is_native() )
                     {
                        return fn->atom.native( args, *context, io );
                     }
                     else if( fn->is_lambda() )
                     {
                        Context * new_env = gc::alloc<Context>( fn->atom.lambda.closure );
                        bind_params( new_env, fn->atom.lambda.params, args );

                        expr    = fn->atom.lambda.body->cons.car;
                        context = new_env;
                        continue;
                     }
                     else
                     {
                        return fn;
                     }
                  }
                  else
                  {
                     return fn;
                  }
               }
            }
         default :
            assert( false );
            io.err << "unhandled-type" << std::endl;
            return make_nil();
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

   if( !res->is_void() )
   {

      res->print( io );
      if( flags & FLAG_NEWLINE )
      {
         io.out << std::endl;
      }
   }

   return 0;
}

///////////////////////////////////////////////////////////////////////////////

int eval( const std::string & source )
{
   IO io;
   Context ctx;
   return eval( source, ctx, io, FLAG_NEWLINE );
}

///////////////////////////////////////////////////////////////////////////////

const std::string DBG_CMD = "dbg";

///////////////////////////////////////////////////////////////////////////////

int repl()
{
   IO io;
   Context ctx;
   std::string line;
   int res     = 0;
   Flags flags = FLAG_NEWLINE;

   do
   {
      std::cout << "> ";
      if( !std::getline( std::cin, line ) )
      {
         break;
      }

      if( line == DBG_CMD )
      {
         Flags debug_flags = ( FLAG_DUMP_TOKENS | FLAG_DUMP_AST | FLAG_DUMP_ENV );
         flags ^= debug_flags;
         io.out << "toggle-debug-mode" << std::endl;
      }
      else
      {
         res = eval( line, ctx, io, flags );
      }

   } while( ( res == 0 ) && ( !ctx.exit ) );
   return ( ctx.exit_code == 0 ) ? res : ctx.exit_code;
}

} // namespace lisp
