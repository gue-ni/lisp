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
#if 1
   Expr * head = nullptr;
   Expr * tail = nullptr;

   Expr * expr = nullptr;

   while( ( expr = parse_expr() ) )
   {

      Expr * node = make_cons( expr, make_nil() );
      if( !head )
      {

         head = node;
         tail = node;
      }
      else
      {
         tail->cons.cdr = node;
         tail           = node;
      }
   }

   return ( head != nullptr ) ? head : make_nil();
#else
   return parse_expr();
#endif
}

Expr * Parser::parse_expr()
{
   Token tkn = peek();

   switch( tkn.type )
   {
      case TokenType ::LPAREN :
         {
            advance();
            return parse_list();
         }
      case TokenType ::TRUE :
         {
            advance();
            return make_boolean( true );
         }
      case TokenType ::FALSE :
         {
            advance();
            return make_boolean( false );
         }
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
      case TokenType ::QUOTE :
         {
            advance();
            Expr * quote  = make_symbol( "quote" );
            Expr * quoted = parse_expr();
            return make_cons( quote, make_cons( quoted, make_nil() ) );
         }
      case TokenType ::DEFINE :
         {
            advance();
            Expr * keyword = make_symbol( "define" );
            Expr * symbol  = parse_expr();
            Expr * value   = parse_expr();
            return make_cons( keyword, make_cons( symbol, make_cons( value, make_nil() ) ) );
         }
      case TokenType ::LAMBDA :
         {
            advance();
            return parse_lambda();
         }
      default :
         return nullptr;
   }
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

Expr * Parser::parse_lambda()
{

   Expr * keyword = make_symbol( "lambda" );
   Expr * params;

   if( match( TokenType::LPAREN ) )
   {
      params = parse_list();
   }
   else
   {
      assert( false );
   }

   Expr * body = parse_expr();

   return make_cons( keyword, make_cons( params, make_cons( body, make_nil() ) ) );
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
