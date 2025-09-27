#include "logger.h"

#include <cassert>
#include <iostream>

namespace lisp
{

std::string to_string( LogLevel level )
{
   switch( level )
   {
      case lisp::LL_DEBUG :
         return "DEBUG";
      case lisp::LL_INFO :
         return "INFO";
      case lisp::LL_WARNING :
         return "WARNING";
      case lisp::LL_ERROR :
         return "ERROR";
      default :
         assert( false && "unreachable" );
         return "";
   }
}

Logger log = Logger( LL_DEBUG );

} // namespace lisp