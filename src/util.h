#pragma once

#include <iostream>
#include <ostream>
#ifdef __linux__
#include <unistd.h>
#endif

namespace lisp
{

struct IO
{
   std::ostream & out;
   std::ostream & err;
   int pipe_stdin;
   int pipe_stdout;
   IO( std::ostream & o = std::cout, std::ostream & e = std::cerr )
       : out( o )
       , err( e )
#ifdef __linux__
       , pipe_stdin(STDIN_FILENO)
       , pipe_stdout(STDOUT_FILENO)
#endif
   {
   }
};
} // namespace lisp