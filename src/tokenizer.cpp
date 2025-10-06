#include "tokenizer.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <map>

namespace lisp
{

Tokens tokenize( const std::string & source )
{
  Tokenizer tkn( source );
  tkn.run();
  return tkn.tokens();
}

std::map<std::string, TokenType> keywords
    = { { KW_QUOTE, TokenType::QUOTE },     { KW_NIL, TokenType::NIL },
        { KW_TRUE, TokenType::TRUE },       { KW_T, TokenType::TRUE },
        { KW_FALSE, TokenType::FALSE },     { KW_QUASIQUOTE, TokenType::QUASIQUOTE },
        { KW_UNQUOTE, TokenType::UNQUOTE }, { KW_UNQUOTE_SPLICE, TokenType::UNQUOTE_SPLICING } };

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
  while( !is_finished() )
  {
    if( isspace( peek() ) )
    {
      ( void ) next();
    }
    else if( peek() == ';' )
    {
      auto end = std::find( m_current, m_source.cend(), '\n' );
      if( end == m_source.cend() )
      {
        m_current = m_source.cend();
      }
      else
      {
        m_current = end + 1;
      }
    }
    else
    {
      break;
    }
  }
}

bool Tokenizer::is_finished() const
{
  return m_current == m_source.end();
}

void Tokenizer::handle_string()
{
  auto end = std::find( m_current, m_source.cend(), '\"' );

  if( end == m_source.cend() )
  {
    std::cerr << "Expected \" at end of string" << std::endl;
    assert( false );
  }

  std::string str( m_current, end );

  // un-escape newline literals
  std::string::size_type pos = 0;
  while( ( pos = str.find( "\\n", pos ) ) != std::string::npos )
  {
    str.replace( pos, 2, "\n" );
    ++pos;
  }

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

void Tokenizer::handle_identifier( bool is_rest )
{
  auto start = m_current - 1;

  // end of identifier
  std::vector<char> chars = { ' ', ')', '\n' };

  auto end = std::find_if(
      start, m_source.cend(), [&]( char c ) { return std::find( chars.begin(), chars.end(), c ) != chars.end(); } );

  std::string identifier( start, end );

  if( is_rest )
  {
    identifier = "&" + identifier;
  }

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
          push( Token( QUOTE, KW_QUOTE ) );
          break;
        }
      case '`' :
        {
          push( Token( QUASIQUOTE, KW_QUASIQUOTE ) );
          break;
        }
      case ',' :
        {
          if( peek() == '@' )
          {
            next();
            push( Token( UNQUOTE_SPLICING, KW_UNQUOTE_SPLICE ) );
          }
          else
          {
            push( Token( UNQUOTE, KW_UNQUOTE ) );
          }
          break;
        }
      case '&' :
        {
          if( isspace( peek() ) )
          {
            skip_whitespace();
          }
          next();
          handle_identifier( true );
          break;
        }
      case '\"' :
        {
          handle_string();
          break;
        }
      default :
        {
          if( isdigit( c ) || ( ( c == '-' ) && isdigit( peek() ) ) )
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
