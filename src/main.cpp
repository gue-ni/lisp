#include "eval.h"
#include "lisp.h"

#include <fstream>
#include <sstream>

class ArgParser
{
 public:
   struct Option
   {
      bool is_flag;
      bool is_required;
      bool is_present;
      std::string value_str;
   };

   ArgParser()
   {
   }

   void add_argument( const std::string & name, bool is_flag, bool is_required, const std::string & default_value = "" )
   {
      Option opt;
      opt.is_flag     = is_flag;
      opt.is_required = is_required;
      opt.value_str   = default_value;
      opt.is_present  = false;
      m_options[name] = opt;
   }

   void get_argument( const std::string & name, std::string & value )
   {
      auto opt_it = m_options.find( name );
      if( opt_it != m_options.end() )
      {
         value = opt_it->second.value_str;
         return;
      }

      auto pos_it = std::find_if( m_positional.begin(), m_positional.end(), MatchFirst{ name } );
      if( pos_it != m_positional.end() )
      {
         value = pos_it->second;
         return;
      }

      std::cerr << __FUNCTION__ << " ERROR: unknown option " << name << std::endl;
   }

   void get_argument( const std::string & name, bool & value )
   {
      auto opt_it = m_options.find( name );
      if( opt_it != m_options.end() )
      {
         value = opt_it->second.is_present;
         return;
      }

      std::cerr << __FUNCTION__ << " ERROR: unknown option " << name << std::endl;
   }

   void add_argument( const std::string & name )
   {
      m_positional.push_back( std::make_pair( name, "" ) );
   }

   void parse_args( int argc, char ** argv )
   {
      int pos_args = 0;
      for( int i = 1; i < argc; i++ )
      {
         std::string arg = argv[i];
         // std::cout << "ARG: " << arg << std::endl;

         if( arg.rfind( "--", 0 ) == 0 )
         {
            std::string key = arg.substr( 2 );
            // std::cout << "KEY: " << key << std::endl;

            auto it = m_options.find( key );
            if( it != m_options.end() )
            {
               if( it->second.is_flag )
               {
                  // std::cout << "is present" << std::endl;
                  it->second.is_present = true;
               }
               else
               {
                  std::string value = argv[i + 1];
                  // std::cout << key << ": " << value << std::endl;
                  it->second.value_str = value;
                  i++;
               }
            }
            else
            {
               std::cerr << __FUNCTION__ << " ERROR: unknown option " << key << std::endl;
            }
         }
         else
         {
            if( pos_args < m_positional.size() )
            {
               m_positional[pos_args++].second = arg;
            }
            else
            {
               std::cerr << __FUNCTION__ << " ERROR: too many positional arguments" << std::endl;
            }
         }
      }
   }

 private:
   std::map<std::string, Option> m_options;
   std::vector<std::pair<std::string, std::string>> m_positional;

   struct MatchFirst
   {
      std::string value;
      bool operator()( const std::pair<std::string, std::string> & pair )
      {
         return value == pair.first;
      }
   };
};

int disassemble( const std::string & program )
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
   args.add_argument( "loglevel", false, false, "DEBUG" );

   args.parse_args( argc, argv );

   bool dump_as_json;
   args.get_argument( "json", dump_as_json );

   std::string loglevel;
   args.get_argument( "loglevel", loglevel );

   if( 1 < argc )
   {
      std::string filename;
      args.get_argument( "filename", filename );
      std::ifstream file( filename );
      if( !file )
      {
         std::cerr << "Could not open " << filename << std::endl;
         return 1;
      }

      std::ostringstream ss;
      ss << file.rdbuf();
      std::string program = ss.str();

      if( dump_as_json )
      {
         return disassemble( program );
      }

      return lisp::eval( program );
   }
   else
   {
      return lisp::repl();
   }
}
