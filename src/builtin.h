#pragma once

#include "util.h"

namespace lisp
{

///////////////////////////////////////////////////////////////////////////////

struct Expr;
class Context;

///////////////////////////////////////////////////////////////////////////////

typedef Expr * ( *Native )( Expr *, Context &, const IO & io );

///////////////////////////////////////////////////////////////////////////////

namespace builtin
{

Expr * f_str( Expr * arg, Context & context, const IO & io );

Expr * f_print( Expr * arg, Context & context, const IO & io );

Expr * f_println( Expr * arg, Context & context, const IO & io );

Expr * f_to_json( Expr * arg, Context & context, const IO & io );

Expr * f_add( Expr * arg, Context & context, const IO & io );

Expr * f_sub( Expr * arg, Context & context, const IO & io );

Expr * f_mul( Expr * arg, Context & context, const IO & io );

Expr * f_div( Expr * arg, Context & context, const IO & io );

Expr * f_car( Expr * arg, Context & context, const IO & io );

Expr * f_cdr( Expr * arg, Context & context, const IO & io );

Expr * f_cons( Expr * arg, Context & context, const IO & io );

Expr * f_lt( Expr * arg, Context & context, const IO & io );

Expr * f_le( Expr * arg, Context & context, const IO & io );

Expr * f_gt( Expr * arg, Context & context, const IO & io );

Expr * f_ge( Expr * arg, Context & context, const IO & io );

Expr * f_eq( Expr * arg, Context & context, const IO & io );

Expr * f_not( Expr * arg, Context & context, const IO & io );

Expr * f_is_null( Expr * arg, Context & context, const IO & io );

Expr * f_is_number( Expr * arg, Context & context, const IO & io );

Expr * f_is_string( Expr * arg, Context & context, const IO & io );

Expr * f_is_error( Expr * arg, Context & context, const IO & io );

Expr * f_eval( Expr * arg, Context & context, const IO & io );

Expr * f_read( Expr * arg, Context & context, const IO & io );

Expr * f_read_file( Expr * arg, Context & context, const IO & io );

Expr * f_exit( Expr * arg, Context & context, const IO & io );

Expr * f_error( Expr * arg, Context & context, const IO & io );

Expr * f_list( Expr * arg, Context & context, const IO & io );

Expr * f_append( Expr * arg, Context & context, const IO & io );

Expr * f_length( Expr * arg, Context & context, const IO & io );

Expr * f_filter( Expr * arg, Context & context, const IO & io );

Expr * f_map( Expr * arg, Context & context, const IO & io );

Expr * f_reduce( Expr * arg, Context & context, const IO & io );

Expr * f_every( Expr * arg, Context & context, const IO & io );

Expr * f_some( Expr * arg, Context & context, const IO & io );

} // namespace builtin

///////////////////////////////////////////////////////////////////////////////

} // namespace lisp