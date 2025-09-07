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

void Context::define( const char * symbol, Expr * expr )
{
   std::string key( symbol );
   m_env[key] = expr;
}

void Context::print( const IO & io )
{
   for( auto it = m_env.begin(); it != m_env.end(); it++ )
   {
      io.out << it->first << " ";
      print_expr( it->second, io );
      io.out << std::endl;
   }
}

void Context::load_runtime()
{
   define( "+", make_native( builtin::add ) );
   define( "*", make_native( builtin::mult ) );
   define( "print", make_native( builtin::print ) );
   define( "define", make_native( builtin::define ) );
   define( "quote", make_native( builtin::quote ) );
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

bool is_symbol( Expr * expr, const char * symbol )
{
   return ( expr->type == Expr::EXPR_ATOM ) && ( expr->atom.type == Atom::ATOM_SYMBOL )
          && ( strcmp( expr->atom.symbol, symbol ) == 0 );
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
   //  TODO

   if( fn->type == Expr::EXPR_ATOM )
   {
      Atom atom = fn->atom;
      if( atom.type == Atom::ATOM_NATIVE )
      {
         return atom.native( args, context, io );
      }
      else
      {
         return make_expr( atom );
      }
   }
   else
     {
     return make_error("not-a-function");
   }

   return make_void();
}

Expr * eval_cons_2( Expr * expr, Context & context, const IO & io )
{
   assert( expr->type == Expr::EXPR_CONS );

   Cons cons   = expr->cons;
   Expr * op   = cons.car;
   Expr * args = cons.cdr;

   if( is_symbol( op, "quote" ) )
   {
      return args->cons.car;
   }
   else if( is_symbol( op, "define" ) )
   {
      Expr * var   = args->cons.car;
      Expr * value = eval( args->cons.cdr->cons.car, context, io );
      context.define( var->atom.symbol, value );
      return make_void();
   }
   else
   {
      Expr * fn = eval( op, context, io );
      args      = eval_list( args, context, io );
      return apply( fn, args, context, io );
   }
}

Expr * eval_cons( Expr * expr, Context & context, const IO & io )
{
   assert( expr->type == Expr::EXPR_CONS );

   Cons cons   = expr->cons;
   Expr * op   = cons.car;
   Expr * args = cons.cdr;

#if 0
   if (is_symbol(op, "define")) {
     int x = 0;
   } else if (is_symbol(op, "quote")) {
    int x = 1; 
   } else {
     Expr* fn = eval(op, context, io);
     Expr* eval_args = eval_list(args, context, io);
     return apply(fn, eval_args, context, io);
   }
#endif

   Expr * fn = eval( op, context, io );

   if( fn->type == Expr::EXPR_ATOM )
   {
      Atom atom = fn->atom;
      if( atom.type == Atom::ATOM_NATIVE )
      {
         return atom.native( args, context, io );
      }
      else
      {
         return new Expr( atom );
      }
   }
#if 0
   else
   {
      return eval_list( fn, context, io );
   }
#endif
   // TODO: should i execute the second one as well?

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
            return eval_cons_2( expr, context, io );
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

   Expr * program = parse( tokens );
   if( !program )
   {
      return 2;
   }

#if 0
   io.out << "-----" << std::endl;
   print_debug( io.out, program );
   io.out << std::endl;
   io.out << "-----" << std::endl;
#endif

   Expr * res = eval_program( program, context, io );
   if( !res )
   {
      return 3;
   }

   print_expr( res, io );
   if( newline )
   {
      io.out << std::endl;
   }

#if 0
   // print context
   io.out << "-----" << std::endl;
   context.print( io );
   io.out << "-----" << std::endl;
#endif
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