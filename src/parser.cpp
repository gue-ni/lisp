#include "parser.h"

namespace lisp
{

Expr * parse( const Tokens & tokens )
{
   Parser parser( tokens );
   return parser.parse_program();
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

Expr * Parser::parse_program()
{
   Expr *head, *tail, *expr;
   expr = head = tail = nullptr;

   ListBuilder builder;

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

      builder.append( expr );

   } while( true );

   return ( builder.list() != nullptr ) ? builder.list() : make_nil();
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
            if( tkn.is_symbol( KW_DEFUN ) )
            {
               Expr * fn_name = parse_expr();
               if( fn_name->is_error() )
                  return fn_name;

               expect( TokenType::LPAREN );
               m_parenthesis_depth++;

               Expr * fn_params = parse_list();
               if( fn_params->is_error() )
                  return fn_params;

               Expr * fn_body = parse_expr();
               if( fn_body->is_error() )
                  return fn_body;

               return make_list(
                   make_symbol( KW_DEFINE ), fn_name, make_list( make_symbol( KW_LAMBDA ), fn_params, fn_body ) );
            }
            else if( tkn.is_symbol( KW_DEFMACRO ) )
            {
               Expr * macro_name = parse_expr();
               if( macro_name->is_error() )
                  return macro_name;

               expect( TokenType::LPAREN );
               m_parenthesis_depth++;

               Expr * macro_params = parse_list();
               if( macro_params->is_error() )
                  return macro_params;

               Expr * macro_body = parse_expr();
               if( macro_body->is_error() )
                  return macro_body;

               return make_list(
                   make_symbol( KW_DEFINE ), macro_name,
                   make_list( make_symbol( KW_MACRO ), macro_params, macro_body ) );
            }
            else if( tkn.is_symbol( KW_DEFINE ) )
            {
               Expr * symbol = parse_expr();
               if( symbol->is_error() )
                  return symbol;

               Expr * value = parse_expr();
               if( value->is_error() )
                  return value;

               return make_list( make_symbol( KW_DEFINE ), symbol, value );
            }
            else
            {
               return make_symbol( tkn.lexeme.c_str() );
            }
         }
      case TokenType ::QUOTE :
      case TokenType ::UNQUOTE :
      case TokenType ::UNQUOTE_SPLICING :
      case TokenType ::QUASIQUOTE :
         {
            advance();
            Expr * quote  = make_symbol( tkn.lexeme.c_str() );
            Expr * quoted = parse_expr();
            if( quoted->is_error() )
               return quoted;

            return make_list( quote, quoted );
         }

      case TokenType ::LAMBDA :
         {
            advance();
            return parse_lambda();
         }
      case TokenType ::NIL :
         {
            advance();
            return make_nil();
         }
      default :
         assert( false );
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
   Expr * keyword = make_symbol( KW_LAMBDA );
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

   return make_list( keyword, params, body );
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

bool Parser::expect( TokenType type )
{
   if( peek().type == type )
   {
      advance();
      return true;
   }
   else
   {
      assert( false );
      return false;
   }
}

Token Parser::peek()
{
   return *( m_current );
}

} // namespace lisp
