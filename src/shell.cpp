#include "shell.h"
#include "eval.h"
#include "expr.h"
#include <cstddef>
#include <cstdio>
#include <string>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

namespace lisp
{

namespace shell
{

Expr * f_exec( Expr * arg, Context & context, const IO & io )
{

#if 0
   Expr * local_stdin  = context.lookup( "*stdin-fd*" );
   Expr * local_stdout = context.lookup( "*stdout-fd*" );
   int stdin_fd  = ( local_stdin->is_integer() ) ? local_stdin->as_integer() : STDIN_FILENO;
   int stdout_fd = ( local_stdout->is_integer() ) ? local_stdout->as_integer() : STDOUT_FILENO;
#else
   int stdin_fd  = io.pipe_stdin;
   int stdout_fd = io.pipe_stdout;
#endif
   int stderr_fd = STDERR_FILENO;

   // printf("%s stdin-fd=%d stdout-fd=%d\n", __PRETTY_FUNCTION__, stdin_fd,
   //        stdout_fd);

   pid_t pid;
   int status = -1;

   // first two arguments are file descriptors

   errno = 0;
   pid   = fork();

   if( pid == 0 )
   {

      // prepare args
      std::vector<char *> argv;
      for( Expr * it = arg; it->is_cons(); it = it->cdr() )
      {
         Expr * el  = it->car();
         Expr * str = el->is_string() ? el : cast_to_string( el );
         argv.push_back( const_cast<char *>( str->as_string() ) );
      }
      argv.push_back( nullptr );

      dup2( stdin_fd, STDIN_FILENO );
      dup2( stdout_fd, STDOUT_FILENO );
      dup2( stderr_fd, STDERR_FILENO );

      // close extra fds
      for( int fd : { stdin_fd, stdout_fd, stderr_fd } )
      {
         if( fd > 2 )
            close( fd );
      }

      execvp( argv[0], argv.data() );

      if( errno != 0 )
      {
         return make_error( strerror( errno ) );
      }
   }
   else if( pid > 0 )
   {
      if( stdin_fd != STDIN_FILENO )
      {
         close( stdin_fd );
      }
      if( stdout_fd != STDOUT_FILENO )
      {
         close( stdout_fd );
      }
      waitpid( pid, &status, 0 );
   }
   else
   {
      perror( "fork failed" );
      return make_error( strerror( errno ) );
   }

   // printf("pid=%d %d %d %d status=%d\n", pid, stdin_fd, stdout_fd, stderr_fd,
   //        status);

   // return make_list(
   //     make_integer( status ), make_integer( pid ), make_integer( stdin_fd ), make_integer( stdout_fd ),
   //     make_integer( stderr_fd ) );

   return make_integer( status );
}

} // namespace shell

} // namespace lisp