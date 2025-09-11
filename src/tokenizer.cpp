#include "tokenizer.h"

#include <algorithm>
#include <cassert>
#include <map>

namespace lisp
{

Tokens tokenize( const std::string & source )
{
   Tokenizer tkn( source );
   tkn.run();
   return tkn.tokens();
}

std::map<std::string, TokenType> keywords = {
    { "lambda", TokenType::LAMBDA }, { "define", TokenType::DEFINE }, { "quote", TokenType::QUOTE },
    { "nil", TokenType::NIL },       { "true", TokenType::TRUE },     { "false", TokenType::FALSE },
};

Tokenizer::Tokenizer( const std::string & source )
    : m_source( source )
    , m_current( m_source.begin() )
{
}

char Tokenizer::next()
{
   if( !is_finished() )
   {
      return *( m_current++ );
   }
   else
   {
      return '\0';
   }
}

char Tokenizer::peek()
{
   assert( !is_finished() );
   return *m_current;
}

char Tokenizer::peek_next()
{
   return *( m_current + 1 );
}

void Tokenizer::skip_whitespace()
{
   // TODO: skip comments
   while( !is_finished() && isspace( peek() ) )
   {
      ( void ) next();
   }
}

bool Tokenizer::is_finished() const
{
   return m_current == m_source.end();
}

void Tokenizer::handle_string()
{
   auto start = m_current - 1;
   auto end   = std::find( m_current, m_source.cend(), '\"' );

   if( end == m_source.cend() )
   {
      assert( false );
   }

   std::string str( m_current, end );

   push( Token( STRING, str ) );
   m_current = end + 1;
}

void Tokenizer::handle_number()
{
   std::string::const_iterator start = m_current - 1;

   while( !is_finished() && std::isdigit( peek() ) )
   {
      next();
   }

   if( !is_finished() && peek() == '.' && std::isdigit( peek_next() ) )
   {
      next();
   }

   while( !is_finished() && std::isdigit( peek() ) )
   {
      next();
   }

   std::string number( start, m_current );
   m_tokens.push_back( Token( NUMBER, number ) );
}

void Tokenizer::handle_identifier()
{
   auto start = m_current - 1;

   // end of identifier
   std::string chars = " )\n";
   auto end = std::find_if( start, m_source.cend(), [&]( char c ) { return chars.find( c ) != std::string::npos; } );

   std::string identifier( start, end );

   auto it = keywords.find( identifier );

   if( it != keywords.end() )
   {
      m_tokens.push_back( Token( it->second, identifier ) );
   }
   else
   {
      m_tokens.push_back( Token( SYMBOL, identifier ) );
   }

   m_current = end;
}

void Tokenizer::push( const Token & token )
{
   m_tokens.push_back( token );
}

void Tokenizer::run()
{
   while( ( !is_finished() ) )
   {
      skip_whitespace();
      char c = next();

      if( c == '\0' )
      {
         break;
      }

      switch( c )
      {
         case '(' :
            {
               m_tokens.push_back( Token( LPAREN, c ) );
               break;
            }
         case ')' :
            {
               m_tokens.push_back( Token( RPAREN, c ) );
               break;
            }
         case '\'' :
            {
               push( Token( QUOTE, c ) );
               break;
            }

         case '\"' :
            {
               handle_string();
               break;
            }
         default :
            {
               if( isdigit( c ) )
               {
                  handle_number();
               }
               else
               {
                  handle_identifier();
               }
               break;
            }
      }
   }

   m_tokens.push_back( Token( END, "(end)" ) );
}

Tokens Tokenizer::tokens()
{
   return m_tokens;
}

} // namespace lisp
