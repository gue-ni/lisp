#pragma once

#include <iostream>
#include <ostream>

namespace lisp
{

struct IO
{
   std::ostream & out;
   std::ostream & err;
   IO( std::ostream & o = std::cout, std::ostream & e = std::cerr )
       : out( o )
       , err( e )
   {
   }
};
} // namespace lisp