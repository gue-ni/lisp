#pragma once

#include "util.h"

namespace lisp
{

///////////////////////////////////////////////////////////////////////////////

struct Expr;
class Context;

///////////////////////////////////////////////////////////////////////////////

typedef Expr * ( *NativeFunction )( Expr *, Context &, const IO & io );

///////////////////////////////////////////////////////////////////////////////

namespace builtin
{

Expr * f_print( Expr * arg, Context & context, const IO & io );

Expr * f_println( Expr * arg, Context & context, const IO & io );

Expr * f_print_debug( Expr * arg, Context & context, const IO & io );

Expr * f_add( Expr * arg, Context & context, const IO & io );

Expr * f_sub( Expr * arg, Context & context, const IO & io );

Expr * f_mul( Expr * arg, Context & context, const IO & io );

Expr * f_div( Expr * arg, Context & context, const IO & io );

Expr * f_car( Expr * arg, Context & context, const IO & io );

Expr * f_cdr( Expr * arg, Context & context, const IO & io );

Expr * f_cons( Expr * arg, Context & context, const IO & io );

Expr * f_lt( Expr * arg, Context & context, const IO & io );

Expr * f_gt( Expr * arg, Context & context, const IO & io );

Expr * f_eq( Expr * arg, Context & context, const IO & io );

Expr * f_and( Expr * arg, Context & context, const IO & io );

Expr * f_or( Expr * arg, Context & context, const IO & io );

Expr * f_not( Expr * arg, Context & context, const IO & io );

Expr * f_empty( Expr * arg, Context & context, const IO & io );

Expr * f_eval( Expr * arg, Context & context, const IO & io );

Expr * f_read( Expr * arg, Context & context, const IO & io );

} // namespace builtin

///////////////////////////////////////////////////////////////////////////////

} // namespace lisp