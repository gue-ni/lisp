#include "expr.h"
#include "eval.h"

namespace lisp
{

std::list<Expr *> GC::heap;

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



void Expr::print( const IO & io ) const
{
   switch( type )
   {
      case Expr::EXPR_ATOM :
         {
            atom.print( io );
            break;
         }
      case Expr::EXPR_CONS :
         {
            io.out << "(";
            cons.print( io );
            io.out << ")";
            break;
         }
      default :
         // do nothing
         break;
   }
}

void Expr::print_debug( std::ostream& os ) const
{
   switch( type )
   {
      case Expr ::EXPR_ATOM :
         {
            switch( atom.type )
            {
               case Atom ::ATOM_NIL :
                  {
                    os  << "Nil";
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
            cons.car->print_debug(os);
            os << ", ";
            cons.cdr->print_debug(os);
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

bool Expr::is_symbol( const char * symbol ) const
{
   return ( is_atom() ) && ( atom.type == Atom::ATOM_SYMBOL ) && ( strcmp( atom.symbol, symbol ) == 0 );
}

bool Expr::is_nil() const
{
   return ( is_atom() ) && ( atom.type == Atom::ATOM_NIL );
}

void GC::garbage_collection()
{
#if 1

   int deleted = 0;

   // sweep
   for( auto it = GC::heap.begin(); it != GC::heap.end(); )
   {
      Expr * expr = *it;
      if( !expr->marked )
      {
         printf( "free Expr(%p)\n", ( void * ) expr );
         deleted++;
         delete expr;
         expr = nullptr;
         it   = GC::heap.erase( it );
      }
      else
      {
         expr->marked = false;
         it++;
      }
   }

   std::cout << "Objects: " << GC::heap.size() << ", deleted: " << deleted << std::endl;

#endif
}

void GC::mark( Expr * expr )
{
   expr->marked = true;
   if( expr->is_cons() )
   {
      mark( expr->cons.cdr );
      mark( expr->cons.car );
   }
}

void Atom::print( const IO & io ) const
{

   switch( type )
   {
      case Atom::ATOM_NIL :
         {
            io.out << "()";
            break;
         }
      case Atom::ATOM_NUMBER :
         {
            io.out << number;
            break;
         }
      case Atom::ATOM_SYMBOL :
         {
            io.out << symbol;
            break;
         }
      case Atom::ATOM_STRING :
         {
            io.out << "\"" << string << "\"";
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
            io.out << "ERROR: " << error;
            break;
         }
      default :
         {
            io.out << "<unprintable>";
         }
   }
}

void Cons::print( const IO & io ) const
{
   car->print( io );

   if( cdr->is_nil() )
   {
      // end of list
      return;
   }
   else if( cdr->is_cons() )
   {
      io.out << " ";
      cdr->cons.print( io );
   }
   else
   {
      io.out << " . ";
      cdr->print( io );
   }
}

} // namespace lisp