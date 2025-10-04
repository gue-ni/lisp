#include "expr.h"
#include "eval.h"
#include "tokenizer.h"
#include "util.h"
#include <sstream>
#include <string>

namespace lisp
{

///////////////////////////////////////////////////////////////////////////////

Expr::~Expr()
{
   switch( type )
   {
      case EXPR_ATOM :
         atom.~Atom();
         break;
      case EXPR_CONS :
      case EXPR_VOID :
         // do nothing
         break;
   }
}

Expr::Expr()
    : gc::Garbage()
    , type( EXPR_VOID )
{
}

Expr::Expr( Atom && a )
    : gc::Garbage()
    , type( EXPR_ATOM )
    , atom( std::move( a ) )
{
}

Expr::Expr( Cons c )
    : gc::Garbage()
    , type( EXPR_CONS )
    , cons( c )
{
}

bool Expr::is_void() const
{
   return type == Expr::EXPR_VOID;
}

bool Expr::is_cons() const
{
   return type == Expr::EXPR_CONS;
}

bool Expr::is_atom() const
{
   return type == Expr::EXPR_ATOM;
}

bool Expr::is_atom( Atom::Type atype ) const
{
   return is_atom() && ( atom.type == atype );
}

bool Expr::is_symbol( const char * symbol ) const
{
   return is_atom() && ( atom.type == Atom::ATOM_SYMBOL ) && ( strcmp( atom.symbol, symbol ) == 0 );
}

bool Expr::is_native() const
{
   return is_atom() && ( atom.type == Atom::ATOM_NATIVE );
}

bool Expr::is_lambda() const
{
   return is_atom() && ( atom.type == Atom::ATOM_LAMBDA );
}

bool Expr::is_macro() const
{
   return is_atom() && ( atom.type == Atom::ATOM_MACRO );
}

bool Expr::is_procedure() const
{
   return is_native() || is_lambda() || is_macro();
}

bool Expr::is_error() const
{
   return is_atom() && ( atom.type == Atom::ATOM_ERROR );
}

bool Expr::is_truthy() const
{
   if( is_cons() )
   {
      return true;
   }
   else if( is_atom() )
   {
      return atom.is_truthy();
   }
   else
   {
      return false;
   }
}

void Expr::mark()
{
   set_marked( true );
   if( is_cons() )
   {
      cons.car->mark();
      cons.cdr->mark();
   }
   else if( is_atom() )
   {
      // closures?
      if( is_lambda() )
      {
#if 0
         for( auto & [k, v] : atom.lambda.closure->env() )
         {
            v->mark();
         }
#endif
      }
   }
}

Expr * Expr::car()
{
   if( is_cons() )
   {
      return cons.car;
   }
   else
   {
      std::cerr << "tried to access car of non-cons" << std::endl;
      assert( false );
      return nullptr;
   }
}

Expr * Expr::cdr()
{
   if( is_cons() )
   {
      return cons.cdr;
   }
   else
   {
      std::cerr << "tried to access cdr of non-cons" << std::endl;
      assert( false );
      return nullptr;
   }
}

const char * Expr::as_symbol() const
{
   if( is_symbol() )
   {
      return atom.symbol;
   }
   else
   {
      assert( false && "unreachable" );
      return nullptr;
   }
}

const char * Expr::as_string() const
{
   if( is_string() )
   {
      return atom.string;
   }
   else
   {
      assert( false && "Expr::as_string() unreachable" );
      return nullptr;
   }
}

int Expr::as_integer() const
{
   if( is_integer() )
   {
      return atom.integer;
   }
   else
   {
      UNREACHABLE
      return -1;
   }
}

double Expr::as_real() const
{
   if( is_real() )
   {
      return atom.real;
   }
   else
   {
      UNREACHABLE
      return -1;
   }
}

double Expr::as_number() const
{
   if( is_integer() )
   {
      return ( double ) atom.integer;
   }
   else if( is_real() )
   {
      return atom.real;
   }
   else
   {
      UNREACHABLE;
      return -1;
   }
}

Expr * cast_to_string( Expr * expr )
{
   if( expr->is_string() )
   {
      return make_string( expr->atom.string );
   }
   else
   {
      std::string str = to_string( expr );
      return make_string( str.c_str() );
   }
}

bool Expr::is_nil() const
{
   return ( is_atom() ) && ( atom.type == Atom::ATOM_NIL );
}

bool Expr::is_string() const
{
   return is_atom() && ( atom.type == Atom::ATOM_STRING );
}

bool Expr::is_real() const
{
   return is_atom() && ( atom.type == Atom::ATOM_REAL );
}

bool Expr::is_integer() const
{
   return is_atom() && ( atom.type == Atom::ATOM_INTEGER );
}

bool Expr::is_number() const
{
   return is_real() || is_integer();
}

bool Expr::is_symbol() const
{
   return is_atom() && ( atom.type == Atom::ATOM_SYMBOL );
}

///////////////////////////////////////////////////////////////////////////////

Atom::Atom()
    : type( ATOM_NIL )
{
}

Atom::~Atom()
{
   switch( type )
   {
      case lisp::Atom::ATOM_NIL :
      case lisp::Atom::ATOM_REAL :
      case lisp::Atom::ATOM_INTEGER :
      case lisp::Atom::ATOM_BOOLEAN :
      case lisp::Atom::ATOM_LAMBDA :
      case lisp::Atom::ATOM_NATIVE :
      case lisp::Atom::ATOM_MACRO :
         break;
      case lisp::Atom::ATOM_SYMBOL :
         if( symbol )
         {
            free( symbol );
         }
         break;
      case lisp::Atom::ATOM_STRING :
         if( string )
         {
            free( string );
         }
         break;
      case lisp::Atom::ATOM_ERROR :
         if( error )
         {
            free( error );
         }
         break;
   }
}

Atom::Atom( Atom && other ) noexcept
    : type( other.type )
{
   switch( type )
   {
      case lisp::Atom::ATOM_NIL :
         break;
      case lisp ::Atom ::ATOM_BOOLEAN :
         boolean = other.boolean;
         break;
      case lisp::Atom::ATOM_REAL :
         real = other.real;
         break;
      case lisp::Atom::ATOM_INTEGER :
         integer = other.integer;
         break;
      case lisp::Atom::ATOM_SYMBOL :
         symbol       = other.symbol;
         other.symbol = nullptr;
         break;
      case lisp::Atom::ATOM_STRING :
         string       = other.string;
         other.string = nullptr;
         break;
      case lisp::Atom::ATOM_LAMBDA :
         lambda           = other.lambda;
         other.lambda.env = nullptr;
         break;
      case lisp::Atom::ATOM_MACRO :
         macro = other.macro;
         break;
      case lisp::Atom::ATOM_NATIVE :
         native = other.native;
         break;
      case lisp::Atom::ATOM_ERROR :
         error       = other.error;
         other.error = nullptr;
         break;
   }
}

bool Atom::is_numeric() const
{
   return type == Atom::ATOM_INTEGER || type == Atom::ATOM_REAL;
}
double Atom::as_numeric() const
{
   if( type == Atom::ATOM_INTEGER )
   {
      return ( double ) integer;
   }
   else if( type == Atom::ATOM_REAL )
   {
      return real;
   }
   else
   {
      UNREACHABLE;
      return 0;
   }
}

bool Atom::is_truthy() const
{
   switch( type )
   {
      case lisp::Atom::ATOM_NIL :
         return false;
      case lisp::Atom::ATOM_BOOLEAN :
         return boolean;
      case lisp::Atom::ATOM_REAL :
         return real != 0;
      case lisp::Atom::ATOM_INTEGER :
         return integer != 0;
      case lisp::Atom::ATOM_STRING :
         return ( string != nullptr ) && ( string[0] != '\0' );
      case lisp::Atom::ATOM_ERROR :
         return false;
      case lisp::Atom::ATOM_SYMBOL :
      case lisp::Atom::ATOM_LAMBDA :
      case lisp::Atom::ATOM_NATIVE :
      case lisp::Atom::ATOM_MACRO :
         return false;
   }

   UNREACHABLE
   return false;
}

std::string Atom::to_json() const
{
   switch( type )
   {
      case Atom::ATOM_NIL :
         return "null";
      case Atom ::ATOM_BOOLEAN :
         return ( boolean ? KW_TRUE : KW_FALSE );
      case Atom::ATOM_REAL :
         return std::to_string( real );
      case Atom::ATOM_INTEGER :
         return std::to_string( integer );
      case Atom::ATOM_SYMBOL :
         {
            std::ostringstream os;
            os << "\"symbol(" << symbol << ")\"";
            return os.str();
         }
      case Atom::ATOM_STRING :
         {
            std::ostringstream os;
            os << "\"" << string << "\"";
            return os.str();
         }
      case Atom::ATOM_LAMBDA :
         return "{ \"lambda\": { \"params\": " + lambda.params->to_json() + ", \"body\": " + lambda.body->to_json()
                + " } }";
      case Atom::ATOM_MACRO :
         return "{ \"macro\": { \"params\": " + macro.params->to_json() + ", \"body\": " + macro.body->to_json()
                + " } }";
      case Atom::ATOM_NATIVE :
         return "\"native()\"";
      case Atom ::ATOM_ERROR :
         return "\"error(" + std::string( error ) + ")\"";
   }

   UNREACHABLE
   return "";
}

bool Atom::operator==( const Atom & other ) const
{
   if( is_numeric() && other.is_numeric() )
   {
      return as_numeric() == other.as_numeric();
   }
   else if( type != other.type )
   {
      return false;
   }

   switch( type )
   {
      case lisp::Atom::ATOM_NIL :
         return true;
      case lisp::Atom::ATOM_BOOLEAN :
         return boolean == other.boolean;
      case lisp::Atom::ATOM_REAL :
         return real == other.real;
      case lisp::Atom::ATOM_INTEGER :
         return integer == other.integer;
      case lisp::Atom::ATOM_SYMBOL :
         return ( strcmp( symbol, other.symbol ) == 0 );
      case lisp::Atom::ATOM_STRING :
         return ( strcmp( string, other.string ) == 0 );
      case lisp::Atom::ATOM_MACRO :
      case lisp::Atom::ATOM_LAMBDA :
         return false;
      case lisp::Atom::ATOM_NATIVE :
         return native == other.native;
      case lisp::Atom::ATOM_ERROR :
         return ( strcmp( error, other.error ) == 0 );
   }

   UNREACHABLE
   return false;
}

bool Atom::operator>( const Atom & other ) const
{
   if( is_numeric() && other.is_numeric() )
   {
      return as_numeric() > other.as_numeric();
   }
   else if( type != other.type )
   {
      return false;
   }

   switch( type )
   {
      case lisp::Atom::ATOM_REAL :
         return real > other.real;
      case lisp::Atom::ATOM_INTEGER :
         return integer > other.integer;
      case lisp::Atom::ATOM_NIL :
      case lisp::Atom::ATOM_BOOLEAN :
      case lisp::Atom::ATOM_SYMBOL :
      case lisp::Atom::ATOM_STRING :
      case lisp::Atom::ATOM_LAMBDA :
      case lisp::Atom::ATOM_NATIVE :
      case lisp::Atom::ATOM_ERROR :
      case lisp::Atom::ATOM_MACRO :
         return false;
   }

   UNREACHABLE
   return false;
}

///////////////////////////////////////////////////////////////////////////////

Cons::Cons( Expr * _car, Expr * _cdr )
    : car( _car )
    , cdr( _cdr )
{
}

std::string Cons::to_json() const
{
   return "{ \"car\": " + car->to_json() + ", \"cdr\": " + cdr->to_json() + " }";
}

///////////////////////////////////////////////////////////////////////////////

std::string Expr::to_json() const
{
   switch( type )
   {
      case Expr::EXPR_ATOM :
         return atom.to_json();
      case Expr::EXPR_CONS :
         return cons.to_json();
      case Expr::EXPR_VOID :
         return "{}";
   }

   UNREACHABLE
   return "";
}

std::string to_string( Expr * expr )
{
   switch( expr->type )
   {
      case Expr::EXPR_ATOM :
         {
            switch( expr->atom.type )
            {
               case Atom ::ATOM_NIL :
                  return "nil";
               case Atom ::ATOM_BOOLEAN :
                  return ( expr->atom.boolean ? KW_TRUE : KW_FALSE );
               case Atom ::ATOM_STRING :
                  return std::string( expr->atom.string );
               case Atom ::ATOM_SYMBOL :
                  return std::string( expr->atom.symbol );
               case Atom ::ATOM_ERROR :
                  return "(error: " + std::string( expr->atom.error ) + ")";
               case Atom ::ATOM_REAL :
                  {
                     std::stringstream ss;
                     ss << expr->atom.real;
                     return ss.str();
                  }
               case Atom ::ATOM_INTEGER :
                  {
                     std::stringstream ss;
                     ss << expr->atom.integer;
                     return ss.str();
                  }
               case Atom::ATOM_LAMBDA :
                  return "(lambda-fn)";
               case Atom::ATOM_NATIVE :
                  return "(native-fn)";
               case Atom::ATOM_MACRO :
                  return "(macro-fn)";
            }

            UNREACHABLE;
            return "";
         }
      case Expr::EXPR_CONS :
         {
            std::string str = "(";
            for( Expr * it = expr; it->is_cons(); it = it->cdr() )
            {
               if( it != expr )
                  str += " ";
               str += to_string( it->car() );
            }
            str += ")";
            return str;
         }
      default :
         return "";
   }
}

std::string to_string_repr( Expr * expr )
{
   switch( expr->type )
   {
      case Expr ::EXPR_ATOM :
         {
            switch( expr->atom.type )
            {
               case Atom ::ATOM_STRING :
                  {
                     std::ostringstream os;
                     os << "\"" << expr->atom.string << "\"";
                     return os.str();
                  }
               case Atom ::ATOM_ERROR :
                  {
                     std::ostringstream os;
                     os << "(error \"" << expr->atom.error << "\")";
                     return os.str();
                  }
               default :
                  return to_string( expr );
            }
         }
      case Expr ::EXPR_CONS :
         {
            Expr * it       = expr;
            std::string str = "(";
            while( true )
            {
               if( it != expr )
                  str += " ";

               str += to_string_repr( it->car() );

               if( it->cdr()->is_nil() )
               {
                  str += ")";
                  break;
               }
               else if( it->cdr()->is_atom() )
               {
                  str += " . " + to_string_repr( it->cdr() ) + ")";
                  break;
               }
               else
               {
                  it = it->cdr();
               }
            }
            return str;
         }
      default :
         return "";
   }
}

} // namespace lisp
