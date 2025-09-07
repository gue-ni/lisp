#include "expr.h"

namespace lisp
{

Expr * NativeFn::operator()( Expr * args, Context & context, const IO & io )
{
   return fn( nullptr, context, io, args );
}

Expr * LambdaFn::operator()( Expr * args, Context & context, const IO & io )
{
   return make_nil();
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

} // namespace lisp