#include "argparser.h"
#include "eval.h"
#include "lisp.h"
#include "version.h"

int compile_and_print( const std::string & program )
{
  lisp::Expr * p = lisp::parse( program );
  if( p == nullptr )
  {
    return 1;
  }
  std::cout << "[";
  for( lisp::Expr * it = p; it->is_cons(); it = it->cdr() )
  {
    std::cout << it->car()->to_json();
    if( !( it->cdr()->is_nil() ) )
      std::cout << ", ";
  }
  std::cout << "]";
  return 0;
}

int main( int argc, char ** argv )
{
  ArgParser args;
  args.add_argument( "filename" );
  args.add_argument( "json", true, false );
  args.add_argument( "version", true, false );
  args.add_argument( "help", true, false );

  args.parse_args( argc, argv );

  bool print_json;
  args.get_argument( "json", print_json );

  bool print_help = false;
  if( args.get_argument( "help", print_help ) && print_help )
  {
    args.print_help();
    return 0;
  }

  bool print_version;
  if( args.get_argument( "version", print_version ) && print_version )
  {
    lisp::print_version_info();
    return 0;
  }

  std::string filename;
  args.get_argument( "filename", filename );

  if( print_json && filename.empty() )
  {
    std::cerr << "'--json' expects a filename to be set" << std::endl;
    return 1;
  }

  if( !filename.empty() )
  {
    std::ifstream file( filename );
    if( !file )
    {
      std::cerr << "Could not open '" << filename << "'" << std::endl;
      return 1;
    }

    std::ostringstream ss;
    ss << file.rdbuf();
    std::string program = ss.str();

    if( print_json )
    {
      return compile_and_print( program );
    }

    return lisp::eval( program, lisp::FLAG_INIT );
  }
  else
  {
    return lisp::repl();
  }
}
