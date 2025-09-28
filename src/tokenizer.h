#pragma once

#include <string>
#include <vector>

// keywords
#define KW_NIL "nil"
#define KW_TRUE "true"
#define KW_FALSE "false"
#define KW_CAR "car"
#define KW_CDR "cdr"
#define KW_CONS "cons"
#define KW_APPEND "append"
#define KW_DEFINE "define"
#define KW_MACRO "macro"
#define KW_LAMBDA "lambda"
#define KW_IF "if"
#define KW_PROGN "progn"
#define KW_LET "let"
#define KW_QUOTE "quote"
#define KW_QUASIQUOTE "quasiquote"
#define KW_UNQUOTE "unquote"
#define KW_UNQUOTE_SPLICE "unquote-splicing"
#define KW_DEFUN "defun"
#define KW_DEFMACRO "defmacro"

namespace lisp
{

enum TokenType
{
   LPAREN,
   RPAREN,
   DOT,
   QUOTE,
   QUASIQUOTE,
   UNQUOTE,
   UNQUOTE_SPLICING,
   NUMBER,
   LAMBDA,
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

   bool is_symbol( const std::string symbol ) const
   {
      return ( type == TokenType::SYMBOL ) && ( lexeme == symbol );
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
   const std::string m_source;
   std::string::const_iterator m_current;
   Tokens m_tokens;

   char next();
   char peek();
   char peek_next();
   void skip_whitespace();
   bool is_finished() const;
   void handle_string();
   void handle_number();
   void handle_identifier( bool is_rest = false );
   void push( const Token & );
};

Tokens tokenize( const std::string & source );

} // namespace lisp