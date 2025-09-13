#include "gc.h"

namespace lisp
{

///////////////////////////////////////////////////////////////////////////////

namespace gc
{
std::list<Garbage *> Garbage::heap;

///////////////////////////////////////////////////////////////////////////////

Garbage::Garbage()
    : m_marked( false )
{
}

bool Garbage::is_marked() const
{
   return m_marked;
}

void Garbage::set_marked( bool b )
{
   m_marked = b;
}

void mark( Garbage * expr )
{
#if 0
   expr->m_marked = true;
   if( expr->is_cons() )
   {
      mark( expr->cons.cdr );
      mark( expr->cons.car );
   }
   else if( expr->is_atom() )
   {
      // closures?
      if( expr->is_lambda() )
      {
         for( auto & [k, v] : expr->atom.lambda.closure->env() )
         {
            mark( v );
         }
      }
   }
#endif
}

void sweep()
{
   auto it = Garbage::heap.begin();
   while( it != Garbage::heap.end() )
   {
      Garbage * g = *it;
      if( !g->is_marked() )
      {
         delete g;
         it = Garbage::heap.erase( it );
      }
      else
      {
         g->set_marked( false );
         ++it;
      }
   }
}

void delete_all()
{
   for( Garbage * g : Garbage::heap )
   {
      delete g;
   }
   Garbage::heap.clear();
}

#if 0
void run( Context & context )
{
   for( const auto & [symbol, expr] : context.env() )
   {
      mark( expr );
   }

   sweep();
}
#endif

} // namespace gc

} // namespace lisp