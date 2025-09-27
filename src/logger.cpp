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

Logger::Logger( LogLevel level )
    : m_level( level )
{
}

const Logger & Logger::instance()
{
   static Logger logger( LL_DEBUG );
   return logger;
}

void Logger::log( LogLevel log_level, const std::string & message ) const
{
   if( m_level <= log_level )
   {
      std::cerr << to_string( log_level ) << " " << message << std::endl;
   }
}

void Logger::log_debug( const std::string & message ) const
{
   log( LL_DEBUG, message );
}

Logger logger = Logger(LL_INFO);

} // namespace lisp