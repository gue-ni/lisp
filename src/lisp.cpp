#include "lisp.h"
#include "builtin.h"
#include "expr.h"
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

Context::Context()
{
   load_runtime();
}

Context::~Context()
{
#if 0
   for( auto it = m_env.begin(); it != m_env.end(); it++ )
   {
      Expr * expr = it->second;
      GC::mark( expr );
   }

   GC::garbage_collection();
#endif
}

Expr * Context::lookup( const char * symbol )
{
   std::string key( symbol );
   auto it = m_env.find( key );
   if( it != m_env.end() )
   {
      return it->second;
   }
   else
   {
      return make_nil();
   }
}

void Context::define( const char * symbol, Expr * expr )
{
   std::string key( symbol );
   m_env[key] = expr;
}

void Context::print( const IO & io )
{
   for( auto it = m_env.begin(); it != m_env.end(); it++ )
   {
      io.out << it->first << " : ";
      print_expr( it->second, io );
      io.out << std::endl;
   }
}

void Context::load_runtime()
{
   define( "+", make_native( builtin::plus ) );
   define( "-", make_native( builtin::minus ) );
   define( "*", make_native( builtin::mult ) );
   define( "/", make_native( builtin::div ) );
   define( "print", make_native( builtin::print ) );
}

Expr * eval_atom( Expr * expr, Context & context, const IO & io )
{
   assert( expr->type == Expr::EXPR_ATOM );
   switch( expr->atom.type )
   {
      case Atom::ATOM_NIL :
      case Atom::ATOM_NUMBER :
      case Atom::ATOM_STRING :
         return expr;
      case Atom::ATOM_SYMBOL :
         return context.lookup( expr->atom.symbol );
      case Atom::ATOM_LAMBDA :
      case Atom::ATOM_NATIVE :
      default :
         return make_nil();
   }
}

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

Expr * apply( Expr * fn, Expr * args, Context & context, const IO & io )
{
   if( ( fn->is_atom() ) && !args->is_nil() )
   {
      if( fn->atom.type == Atom::ATOM_NATIVE )
      {
         return fn->atom.native( args, context, io );
      }
      else if( fn->atom.type == Atom::ATOM_LAMBDA )
      {
         return fn->atom.lambda( args, context, io );
      }
      else
      {
         return fn;
      }
   }

   return fn;
}

Expr * eval_cons( Expr * expr, Context & context, const IO & io )
{
   assert( expr->type == Expr::EXPR_CONS );

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
      Expr * value = eval( args->cons.cdr->cons.car, context, io );
      context.define( var->atom.symbol, value );
      return make_void();
   }
   else if( op->is_symbol( "lambda" ) )
   {
      Expr * params = args->cons.car;
      Expr * body   = args->cons.cdr;
      return make_lambda( params, body );
   }
   else if( op->is_symbol( "car" ) )
   {
      Expr * tmp = eval( args->cons.car, context, io );
      if( !tmp->is_cons() )
      {
         return make_error( "not a cons" );
      }
      return tmp->cons.car;
   }
   else if( op->is_symbol( "cdr" ) )
   {
      Expr * tmp = eval( args->cons.car, context, io );
      if( !tmp->is_cons() )
      {
         return make_error( "not a cons" );
      }
      return tmp->cons.cdr;
   }
   else if( op->is_symbol( "cons" ) )
   {
      Expr * car = eval( args->cons.car, context, io );
      Expr * cdr = eval( args->cons.cdr->cons.car, context, io );
      return make_cons( car, cdr );
   }
   else
   {
      Expr * fn = eval( op, context, io );
      args      = eval_list( args, context, io );
      return apply( fn, args, context, io );
   }
}

Expr * eval( Expr * expr, Context & context, const IO & io )
{
   switch( expr->type )
   {
      case Expr::EXPR_ATOM :
         {
            return eval_atom( expr, context, io );
         }
      case Expr::EXPR_CONS :
         {
            return eval_cons( expr, context, io );
         }
      default :
         io.err << "unhandled-type" << std::endl;
         return make_nil();
   }
}

int eval_print( Expr * expr, Context & context, const IO & io, bool newline )
{
   Expr * res = eval( expr, context, io );
   print_expr( res, io );
   if( newline )
   {
      io.out << std::endl;
   }
   return 0;
}

void print_atom( const Atom & atom, const IO & io )
{
   switch( atom.type )
   {
      case Atom::ATOM_NIL :
         {
            io.out << "()";
            break;
         }
      case Atom::ATOM_NUMBER :
         {
            io.out << atom.number;
            break;
         }
      case Atom::ATOM_SYMBOL :
         {
            io.out << atom.symbol;
            break;
         }
      case Atom::ATOM_STRING :
         {
            io.out << "\"" << atom.string << "\"";
            break;
         }
      case Atom::ATOM_LAMBDA :
         {
            io.out << "<lambda>";
            break;
         }
      case Atom::ATOM_NATIVE :
         {
            io.out << "<native-fn>";
            break;
         }
      case Atom ::ATOM_ERROR :
         {
            io.out << "ERROR: " << atom.error;
            break;
         }
      default :
         {
            io.out << "<unprintable>";
         }
   }
}

void print_cons( Cons cons, const IO & io )
{
   print_expr( cons.car, io );

   if( has_type( cons.cdr, Expr::EXPR_ATOM ) && has_type( cons.cdr->atom, Atom::ATOM_NIL ) )
   {
      // end of list
      return;
   }
   else if( has_type( cons.cdr, Expr::EXPR_CONS ) )
   {
      io.out << " ";
      print_cons( cons.cdr->cons, io );
   }
   else
   {
      io.out << " . ";
      print_expr( cons.cdr, io );
   }
}

void print_expr( Expr * expr, const IO & io )
{
   switch( expr->type )
   {
      case Expr::EXPR_ATOM :
         {
            print_atom( expr->atom, io );
            break;
         }
      case Expr::EXPR_CONS :
         {
            io.out << "(";
            print_cons( expr->cons, io );
            io.out << ")";
            break;
         }
      default :
         // do nothing
         break;
   }
}

void print_debug( std::ostream & os, const Tokens & tokens )
{
   for( const Token & tkn : tokens )
   {
      os << "'" << tkn.lexeme << "'"
         << ", ";
   }
   os << std::endl;
}

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
      print_debug( io.out, program );
      io.out << std::endl;
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

   print_expr( res, io );
   if( !res->is_void() && ( flags & FLAG_NEWLINE ) )
   {
      io.out << std::endl;
   }

   return 0;
}

const std::string DBG_CMD  = "dbg";
const std::string LOAD_CMD = "load-file ";

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
         flags |= ( FLAG_DUMP_TOKENS );
         flags |= ( FLAG_DUMP_AST );
         flags |= ( FLAG_DUMP_ENV );
         io.out << "debug-mode-on" << std::endl;
      }
      else if( line.starts_with( LOAD_CMD ) )
      {
         std::string filename = line.substr( LOAD_CMD.size() );
         std::ifstream file( filename );
         if( !file )
         {
            io.err << "Could not open file " << filename << std::endl;
         }
         else
         {
            io.out << "load-file '" << filename << "'" << std::endl;
            std::ostringstream ss;
            ss << file.rdbuf();
            std::string content = ss.str();

            res = eval( content, ctx, io, flags );
         }
      }
      else
      {
         res = eval( line, ctx, io, flags );
      }

   } while( res == 0 );
   return res;
}

} // namespace lisp