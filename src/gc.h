#pragma once

#include <list>

namespace lisp
{

///////////////////////////////////////////////////////////////////////////////
// garbage collection
namespace gc
{

class Garbage
{
 public:
   Garbage();
   virtual ~Garbage()
   {
   }
   virtual void mark() = 0;
   bool is_marked() const;
   void set_marked( bool );

   static std::list<Garbage *> heap;

 protected:
   bool m_marked;
};

template <typename T, typename... Args>
T * alloc( Args &&... args )
{
   T * obj = new T( std::forward<Args>( args )... );
   Garbage::heap.push_back( obj );
   return obj;
}

void mark( Garbage * );

void sweep();

void run();

void delete_all();

} // namespace gc

} // namespace lisp