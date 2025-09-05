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

   // Expr* args_result = eval_all(cdr, context, io);

   return make_nil();
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

void print( Expr * expr, const IO & io )
{
   switch( expr->type )
   {

      case Expr::EXPR_ATOM :
         {
            Atom atom = expr->atom;
            switch( atom.type )
            {
               case Atom::ATOM_NIL :
                  {
                     io.out << "nil";
                     break;
                  }
               case Atom::ATOM_NUMBER :
                  {
                     io.out << ( atom.number );
                     break;
                  }
               default :
                  io.out << "unprintable-atom";
                  break;
            }
            break;
         }
      case Expr::EXPR_CONS :
         {
            io.err << "unprintable-cons";
            break;
         }
   }
}

void print_atom( const Atom & atom, const IO & io )
{
   switch( atom.type )
   {
      case Atom::ATOM_NIL :
         {

            io.out << "nil";
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
   }
}

void print_cons( Cons cons, const IO & io )
{

   io.out << "(";
   print_expr( cons.car, io );

   while( cons.cdr->type == Expr::EXPR_CONS )
   {
      cons = cons.cdr->cons;
      io.out << " ";
      print_expr( cons.cdr, io );
   }

   io.out << " )";
}

void print_expr( Expr * expr, const IO & io )
{
   if( expr->type == Expr::EXPR_ATOM )
   {
      print_atom( expr->atom, io );
   }
   else
   {
      print_cons( expr->cons, io );
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