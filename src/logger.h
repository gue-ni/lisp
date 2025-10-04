#pragma once

#include <iostream>
#include <sstream>
#include <string>

namespace lisp
{

enum LogLevel : int
{
  LL_NONE,
  LL_DEBUG,
  LL_INFO,
  LL_WARNING,
  LL_ERROR,
};

std::string to_string( LogLevel );

class NullBuffer : public std::streambuf
{
protected:
  int overflow( int c ) override
  {
    return c;
  }
};

class NullStream : public std::ostream
{
public:
  NullStream()
      : std::ostream( &m_sb )
  {
  }

private:
  NullBuffer m_sb;
};

class LogStream : public std::ostream
{
public:
};

class Logger
{
public:
  Logger( LogLevel level )
      : m_level( level )
      , m_logstream( std::cout )
  {
  }

  std::ostream & operator()( LogLevel level = LL_DEBUG )
  {
    return ( m_level <= level ) ? m_logstream : m_nullstream;
  }

  void set_log_level( LogLevel level )
  {
    m_level = level;
  }

private:
  LogLevel m_level;
  std::ostream & m_logstream;
  NullStream m_nullstream;
};

extern Logger log;

} // namespace lisp