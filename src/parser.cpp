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
  #if 0
  Expr* program = 
  Expr* expr = nullptr;
  while ((expr = parse_expr())) {

  }
#else
   return parse_expr();
   #endif
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
#if 0
      case TokenType ::QUOTE :
         {
            advance();
            Expr * quote  = make_symbol( "quote" );
            Expr * quoted = parse_expr();
            return make_cons( quote, make_cons( quoted, make_nil() ));
         }
#endif
#if 1
      case TokenType ::DEFINE :
         {
            advance();
            Expr * keyword = make_symbol( "define" );
            Expr * symbol  = parse_expr();
            Expr * value   = parse_expr();
            // TODO: function definition
            return make_cons( keyword, make_cons( symbol, make_cons( value, make_nil() ) ) );
         }
#endif
      case TokenType ::PRINT :
         {
            advance();
            Expr * keyword = make_symbol( "print" );
            Expr * value   = parse_expr();
            return make_cons( keyword, make_cons( value, make_nil() ) );
         }
      case TokenType ::LPAREN :
         {
            advance();
            return parse_list();
         }
      case TokenType ::CONS :
         {
            advance();
            Expr * car = parse_expr();
            Expr * cdr = parse_expr();
            return make_cons( car, cdr );
         }
      default :
         return nullptr;
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
