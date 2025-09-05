#include "parser.h"

namespace lisp
{

Expr * parse( const Tokens & tokens )
{
   Parser parser( tokens );
   return parser.parse();
}

Parser::Parser( const Tokens & tokens )
    : m_tokens( tokens )
    , m_current( m_tokens.begin() )
{
}

Expr * Parser::parse()
{
   return parse_expr();
}

Expr * Parser::parse_expr()
{
   Token tkn = peek();

   switch( tkn.type )
   {
      case TokenType ::NUMBER :
         {
            advance();
            double num = std::strtod( tkn.lexeme.c_str(), nullptr );
            return make_number( num );
         }
      case TokenType ::SYMBOL :
         {
            advance();
            return make_symbol( tkn.lexeme.c_str() );
         }
      case TokenType ::LPAREN :
         {
            advance();
            return parse_list();
         }
      default :
         return make_nil();
   }
}

Expr * Parser::parse_number()
{
   return nullptr;
}

Expr * Parser::parse_list()
{
   if( match( TokenType::RPAREN ) )
   {
      // emtpy list
      return make_nil();
   }

   Expr * head = parse_expr();
   // TODO: dotted pair?

   Expr * tail = parse_list();
   return make_cons( head, tail );
}

void Parser::advance()
{
   m_current++;
}

bool Parser::match( TokenType type )
{
   if( peek().type == type )
   {
      advance();
      return true;
   }
   else
   {
      return false;
   }
}

Token Parser::peek()
{
   return *( m_current );
}

} // namespace lisp
