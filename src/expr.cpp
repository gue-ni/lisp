#include "expr.h"

#include "lisp.h"

namespace lisp
{

Expr * NativeFn::operator()( Expr * args, Context & context, const IO & io )
{
   return fn( args, context, io );
}

Expr * LambdaFn::operator()( Expr * args, Context & context, const IO & io )
{
   // TODO

   Expr * arg   = args;
   Expr * param = params;

   // TODO: should inherit from context
   // Context closure;

   while( param->is_cons() && arg->is_cons() )
   {
      context.define( param->cons.car->atom.symbol, arg->cons.car );
      arg   = arg->cons.cdr;
      param = param->cons.cdr;
   }

   // TODO: check arity

   Expr * bdy = body->cons.car;

#if 0
   print_debug( std::cout, bdy );
   std::cout << std::endl;
#endif
   
   Expr * result = eval( bdy, context, io );
   return result;
}

void print_debug( std::ostream & os, const Expr * expr )
{
   switch( expr->type )
   {
      case Expr ::EXPR_ATOM :
         {
            const Atom & atom = expr->atom;
            switch( atom.type )
            {
               case Atom ::ATOM_NIL :
                  {
                     os << "Nil";
                     break;
                  }
               case Atom ::ATOM_NUMBER :
                  {
                     os << "Number(" << atom.number << ")";
                     break;
                  }
               case Atom ::ATOM_STRING :
                  {
                     os << "String(\"" << atom.string << "\")";
                     break;
                  }
               case Atom ::ATOM_SYMBOL :
                  {
                     os << "Symbol(" << atom.symbol << ")";
                     break;
                  }
               case Atom ::ATOM_NATIVE :
                  {
                     os << "NativeFn";
                     break;
                  }
               default :
                  os << "Unprintable(" << atom.type << ")";
                  break;
            }
            break;
         }
      case Expr ::EXPR_CONS :
         {
            os << "Cons(";
            print_debug( os, expr->cons.car );
            os << ", ";
            print_debug( os, expr->cons.cdr );
            os << ")";
            break;
         }
      default :
         {
            os << "Void";
            break;
         }
   }
}

bool Expr::is_void() const
{
   return type == Expr::EXPR_VOID;
}

bool Expr::is_cons() const
{
   return type == Expr::EXPR_CONS;
}

bool Expr::is_atom() const
{
   return type == Expr::EXPR_ATOM;
}

} // namespace lisp