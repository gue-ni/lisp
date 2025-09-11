#pragma once

#include "expr.h"
#include "tokenizer.h"

namespace lisp
{

class Parser
{
 public:
   Parser( const Tokens & tokens );
   Expr * parse();

 private:
   Tokens m_tokens;
   Tokens::const_iterator m_current;

   void advance();
   bool match( TokenType );
   Token peek();

   Expr * parse_expr();
   Expr * parse_list();
   Expr * parse_lambda();
};

Expr * parse( const std::string& source );

Expr * parse( const Tokens & tokens );

}; // namespace lisp