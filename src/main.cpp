#include "eval.h"
#include "lisp.h"

#include <fstream>
#include <sstream>

int main( int argc, char ** argv )
{
   if(  1 < argc )
   {
      const char * filename = argv[1];
      std::ifstream file( filename );
      if( !file )
      {
         std::cerr << "Could not open " << filename << std::endl;
         return 1;
      }
      else
      {
         std::ostringstream ss;
         ss << file.rdbuf();
         std::string program = ss.str();
         return lisp::eval( program );
      }
   }
   else
   {
      return lisp::repl();
   }
}
