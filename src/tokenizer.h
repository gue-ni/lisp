#pragma once

#include <string>
#include <vector>

namespace lisp
{

enum TokenType
{
   LPAREN,
   RPAREN,
   DOT,
   QUOTE,
   NUMBER,
   LAMBDA,
   DEFINE,
   SYMBOL,
   STRING,
   TRUE,
   FALSE,
   NIL,
   END,
};

struct Token
{
   TokenType type;
   std::string lexeme;

   Token( TokenType tt )
       : type( tt )
   {
   }

   Token( TokenType tt, char c )
       : type( tt )
       , lexeme( std::string( 1, c ) )
   {
   }

   Token( TokenType tt, const std::string lex )
       : type( tt )
       , lexeme( lex )
   {
   }
};

using Tokens = std::vector<Token>;

class Tokenizer
{
 public:
   Tokenizer( const std::string & source );

   void run();

   Tokens tokens();

 private:
   std::string m_source;
   std::string::const_iterator m_current;
   Tokens m_tokens;

   char next();
   char peek();
   char peek_next();
   void skip_whitespace();
   bool is_finished() const;
   void handle_string();
   void handle_number();
   void handle_identifier();
   void push( const Token & );
};

Tokens tokenize( const std::string & source );

} // namespace lisp