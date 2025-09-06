#include "lisp.h"
#include "builtin.h"
#include "expr.h"
#include "parser.h"
#include "tokenizer.h"

#include <cstdio>
#include <string>
#include <vector>

namespace lisp
{

Context::Context()
{
   load_runtime();
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

void Context::set( const char * symbol, Expr * expr )
{
   std::string key( symbol );
   m_env[key] = expr;
}

void Context::load_runtime()
{
   set( "+", make_native( builtin::add ) );
   set( "*", make_native( builtin::mult ) );
   set( "print", make_native( builtin::print ) );
   set( "define", make_native( builtin::define ) );
   set( "quote", make_native( builtin::quote ) );
   // set( "cons", make_native( builtin::quote ) );
   // set( "car", make_native( builtin::quote ) );
   // set( "cdr", make_native( builtin::quote ) );
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

Expr * eval_cons( Expr * expr, Context & context, const IO & io )
{
   assert( expr->type == Expr::EXPR_CONS );

   Cons cons = expr->cons;

   Expr * fn   = eval( cons.car, context, io );
   Expr * args = cons.cdr;

   if( fn->type == Expr::EXPR_ATOM )
   {
      Atom atom = fn->atom;
      if( atom.type == Atom::ATOM_NATIVE )
      {
         return atom.native( args, context, io );
      }
   }

   // TODO:
   return make_void();
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
      default :
         {
            io.out << "<unprintable>";
         }
   }
}

void print_cons( Cons cons, const IO & io )
{
   // io.out << "(";
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
      // do nothging
         break;
   }
}

int eval( const std::string & source, Context & context, const IO & io, bool newline )
{
   Tokens tokens = tokenize( source );
   if( tokens.empty() )
   {
      return 1;
   }

#if 0
   io.out << "Got " << tokens.size() << " tokens: ";
   for( const Token & tkn : tokens )
   {
      io.out << "'" << tkn.lexeme << "'" << ", ";
   }
   io.out << std::endl;
#endif

   Expr * exp = parse( tokens );
   if( !exp )
   {
      return 2;
   }

#if 0
   io.out << "-----\n";
   print_expr( exp, io );
   io.out << "-----\n";
#endif

   Expr * res = eval( exp, context, io );
   if( !res )
   {
      return 3;
   }

   print_expr( res, io );
   if( newline )
   {
      io.out << std::endl;
   }
   return 0;
}

int repl()
{
   IO io;
   Context ctx;
   std::string line;

   int res = 0;

   do
   {
      std::cout << "> ";

      if( !std::getline( std::cin, line ) )
      {
         break;
      }

      res = eval( line, ctx, io, true );

   } while( res == 0 );

   return 0;
}

} // namespace lisp