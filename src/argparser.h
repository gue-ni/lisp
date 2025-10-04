#pragma once

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>

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

   void print_help()
   {
      for( auto it = m_options.begin(); it != m_options.end(); it++ )
      {
         std::cout << it->first << std::endl;
      }
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

   bool get_argument( const std::string & name, std::string & value )
   {
      auto opt_it = m_options.find( name );
      if( opt_it != m_options.end() )
      {
         value = opt_it->second.value_str;
         return true;
      }

      auto pos_it = std::find_if( m_positional.begin(), m_positional.end(), MatchFirst{ name } );
      if( ( pos_it != m_positional.end() ) && ( !pos_it->second.empty() ) )
      {
         value = pos_it->second;
         return true;
      }

      // std::cerr << "ERROR: unknown option '" << name << "'" << std::endl;
      return false;
   }

   bool get_argument( const std::string & name, bool & value )
   {
      auto opt_it = m_options.find( name );
      if( opt_it != m_options.end() )
      {
         value = opt_it->second.is_present;
         return true;
      }

      std::cerr << " ERROR: unknown option '" << name << "'" << std::endl;
      return false;
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
            if( pos_args < ( int ) m_positional.size() )
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