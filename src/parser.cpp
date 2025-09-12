#include "parser.h"

namespace lisp
{

Expr * parse( const Tokens & tokens )
{
   Parser parser( tokens );
   return parser.parse();
}

///////////////////////////////////////////////////////////////////////////////

Expr * parse( const std::string & source )
{
   Tokens tokens  = tokenize( source );
   Expr * program = parse( tokens );
   return program;
}

Parser::Parser( const Tokens & tokens )
    : m_tokens( tokens )
    , m_current( m_tokens.begin() )
    , m_parenthesis_depth( 0 )
{
}

Expr * Parser::parse()
{
   Expr * head = nullptr;
   Expr * tail = nullptr;
   Expr * expr = nullptr;

   do
   {
      expr = parse_expr();
      if( expr == nullptr )
      {
         break;
      }

      if( expr->is_error() )
      {
         return make_cons( expr, make_nil() );
      }

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
   } while( true );

   return ( head != nullptr ) ? head : make_nil();
}

Expr * Parser::parse_expr()
{
   Token tkn = peek();

   switch( tkn.type )
   {
      case TokenType ::END :
         {
            if( m_parenthesis_depth != 0 )
            {
               return make_error( "missing-parenthesis" );
            }
            return nullptr;
         }
      case TokenType ::LPAREN :
         {
            advance();
            m_parenthesis_depth++;
            return parse_list();
         }
      case TokenType ::RPAREN :
         {
            return make_error( "unexpected-parenthesis" );
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
      case TokenType ::STRING :
         {
            advance();
            return make_string( tkn.lexeme.c_str() );
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
            if( quoted->is_error() )
            {
               return quoted;
            }
            return make_cons( quote, make_cons( quoted, make_nil() ) );
         }
      case TokenType ::DEFINE :
         {
            advance();
            Expr * keyword = make_symbol( "define" );
            Expr * symbol  = parse_expr();
            if( symbol->is_error() )
            {
               return symbol;
            }

            Expr * value = parse_expr();
            if( value->is_error() )
            {
               return value;
            }
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
      m_parenthesis_depth--;
      return make_nil();
   }

   Expr * head = parse_expr();
   if( head->is_error() )
   {
      return head;
   }

   Expr * tail = parse_list();
   if( tail->is_error() )
   {
      return tail;
   }

   return make_cons( head, tail );
}

Expr * Parser::parse_lambda()
{
   Expr * keyword = make_symbol( "lambda" );
   Expr * params;

   if( match( TokenType::LPAREN ) )
   {
      m_parenthesis_depth++;
      params = parse_list();
      if( params->is_error() )
      {
         return params;
      }
   }
   else
   {
      assert( false );
   }

   Expr * body = parse_expr();
   if( body->is_error() )
   {
      return body;
   }

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
