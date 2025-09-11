#include "expr.h"
#include "eval.h"

namespace lisp
{

///////////////////////////////////////////////////////////////////////////////

Expr * NativeFn::operator()( Expr * args, Context & context, const IO & io )
{
   return fn( args, context, io );
}

///////////////////////////////////////////////////////////////////////////////

Expr * LambdaFn::operator()( Expr * args, Context & context, const IO & io )
{
   // TODO

   Expr * arg   = args;
   Expr * param = params;
   Context * local = new Context( closure );

   while( param->is_cons() && arg->is_cons() )
   {
      local->define( param->cons.car->atom.symbol, arg->cons.car );
      arg   = arg->cons.cdr;
      param = param->cons.cdr;
   }

   // TODO: check arity

   Expr * bdy    = body->cons.car;
   Expr * result = make_nil();

   while( bdy->is_cons() )
   {
      result = eval( bdy, *local, io );
      bdy = bdy->cons.cdr->cons.car;
   }

   return result;
}

///////////////////////////////////////////////////////////////////////////////

std::list<Expr *> Expr::all;

Expr::~Expr()
{
   switch( type )
   {
      case EXPR_ATOM :
         atom.~Atom();
         break;
      case EXPR_CONS :
      default :
         // do nothing
         break;
   }
}

Expr::Expr()
    : type( EXPR_VOID )
    , marked( false )
{
   all.push_back( this );
}

Expr::Expr( Atom && a )
    : type( EXPR_ATOM )
    , atom( std::move( a ) )
    , marked( false )
{
   all.push_back( this );
}

Expr::Expr( Cons c )
    : type( EXPR_CONS )
    , cons( c )
    , marked( false )
{
   all.push_back( this );
}

void Expr::print( const IO & io ) const
{
   switch( type )
   {
      case Expr::EXPR_ATOM :
         {
            atom.print( io );
            break;
         }
      case Expr::EXPR_CONS :
         {
            io.out << "(";
            cons.print( io );
            io.out << ")";
            break;
         }
      default :
         // do nothing
         break;
   }
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

bool Expr::is_symbol( const char * symbol ) const
{
   return ( is_atom() ) && ( atom.type == Atom::ATOM_SYMBOL ) && ( strcmp( atom.symbol, symbol ) == 0 );
}

bool Expr::is_lambda() const
{
   return ( is_atom() ) && ( atom.type == Atom::ATOM_LAMBDA );
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

bool Expr::is_nil() const
{
   return ( is_atom() ) && ( atom.type == Atom::ATOM_NIL );
}

bool Expr::is_string() const
{
   return is_atom() && ( atom.type == Atom::ATOM_STRING );
}

bool Expr::is_number() const
{
   return is_atom() && ( atom.type == Atom::ATOM_NUMBER );
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
      case lisp::Atom::ATOM_NUMBER :
      case lisp::Atom::ATOM_BOOLEAN :
      case lisp::Atom::ATOM_LAMBDA :
      case lisp::Atom::ATOM_NATIVE :
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
      case lisp::Atom::ATOM_NUMBER :
         number = other.number;
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
         lambda = other.lambda;
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

bool Atom::is_truthy() const
{
   switch( type )
   {
      case lisp::Atom::ATOM_NIL :
         return false;
      case lisp::Atom::ATOM_BOOLEAN :
         return boolean;
      case lisp::Atom::ATOM_NUMBER :
         return number != 0;
      case lisp::Atom::ATOM_SYMBOL :
      case lisp::Atom::ATOM_STRING :
      case lisp::Atom::ATOM_LAMBDA :
      case lisp::Atom::ATOM_NATIVE :
      case lisp::Atom::ATOM_ERROR :
         return false;
      default :
         assert( false );
         return false;
   }
}

void Atom::print( const IO & io ) const
{
   switch( type )
   {
      case Atom::ATOM_NIL :
         {
            io.out << "()";
            break;
         }
      case Atom ::ATOM_BOOLEAN :
         {
            io.out << ( boolean ? "true" : "false" );
            break;
         }
      case Atom::ATOM_NUMBER :
         {
            io.out << number;
            break;
         }
      case Atom::ATOM_SYMBOL :
         {
            io.out << symbol;
            break;
         }
      case Atom::ATOM_STRING :
         {
            io.out << "\"" << string << "\"";
            break;
         }
      case Atom::ATOM_LAMBDA :
         {
            io.out << "<lambda>";
            break;
         }
      case Atom::ATOM_NATIVE :
         {
            io.out << "<native-fn>";
            break;
         }
      case Atom ::ATOM_ERROR :
         {
            io.out << "(error: " << error << ")";
            break;
         }
      default :
         {
            io.out << "<unprintable>";
         }
   }
}

bool Atom::operator==( const Atom & other ) const
{
   if( type != other.type )
   {
      return false;
   }

   switch( type )
   {
      case lisp::Atom::ATOM_NIL :
         return true;
      case lisp::Atom::ATOM_BOOLEAN :
         return boolean == other.boolean;
      case lisp::Atom::ATOM_NUMBER :
         return number == other.number;
      case lisp::Atom::ATOM_SYMBOL :
         return ( strcmp( symbol, other.symbol ) == 0 );
      case lisp::Atom::ATOM_STRING :
         return ( strcmp( string, other.string ) == 0 );
      case lisp::Atom::ATOM_LAMBDA :
         return false;
      case lisp::Atom::ATOM_NATIVE :
         return native.fn == other.native.fn;
      case lisp::Atom::ATOM_ERROR :
         return ( strcmp( error, other.error ) == 0 );
      default :
         assert( false );
         return false;
   }
}

bool Atom::operator>( const Atom & other ) const
{
   if( type != other.type )
   {
      return false;
   }
   switch( type )
   {
      case lisp::Atom::ATOM_NUMBER :
         return number > other.number;
      case lisp::Atom::ATOM_NIL :
      case lisp::Atom::ATOM_BOOLEAN :
      case lisp::Atom::ATOM_SYMBOL :
      case lisp::Atom::ATOM_STRING :
      case lisp::Atom::ATOM_LAMBDA :
      case lisp::Atom::ATOM_NATIVE :
      case lisp::Atom::ATOM_ERROR :
         return false;
      default :
         assert( false );
         return false;
   }
}

///////////////////////////////////////////////////////////////////////////////

Cons::Cons( Expr * _car, Expr * _cdr )
    : car( _car )
    , cdr( _cdr )
{
}

void Cons::print( const IO & io ) const
{
   car->print( io );

   if( cdr->is_nil() )
   {
      // end of list
      return;
   }
   else if( cdr->is_cons() )
   {
      io.out << " ";
      cdr->cons.print( io );
   }
   else
   {
      io.out << " . ";
      cdr->print( io );
   }
}

///////////////////////////////////////////////////////////////////////////////

} // namespace lisp