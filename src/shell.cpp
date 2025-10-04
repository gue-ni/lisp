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
  pid_t pid;
  int status = -1;

  int stdin_fd  = io.pipe_stdin;
  int stdout_fd = io.pipe_stdout;
  int stderr_fd = STDERR_FILENO;

  errno = 0;
  pid   = fork();

  if( pid == 0 )
  {
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

  return make_boolean( status == 0 );
}

} // namespace shell

} // namespace lisp