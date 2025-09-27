#pragma once

#include <iostream>
#include <sstream>
#include <string>

namespace lisp
{

enum LogLevel : int
{
   LL_DEBUG,
   LL_INFO,
   LL_WARNING,
   LL_ERROR,
};

std::string to_string( LogLevel );

class Logger
{
 public:
   static const Logger & instance();

   void log( LogLevel log_level, const std::string & message ) const;

   void log_debug( const std::string & message ) const;

   template <typename T>
   Logger & operator<<( const T & value )
   {
      if( m_level <= m_message_level )
      {
         std::cerr << value;
      }
      return *this;
   }

   Logger & operator<<( std::ostream & ( *manip )( std::ostream & ) )
   {
      if( m_level <= m_message_level )
      {
         std::cout << manip;
      }
      return *this;
   }

   Logger & operator()( LogLevel level = LL_DEBUG )
   {
      m_message_level = level;
      return *this;
   }

   Logger( LogLevel );

 private:
   LogLevel m_level;
   LogLevel m_message_level;
   std::ostringstream m_buffer;
};

extern Logger logger;

} // namespace lisp