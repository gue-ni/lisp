#pragma once

#include "util.h"

namespace lisp
{

struct Expr;

class Context;

typedef Expr * ( *NativeFunction )( void *, Context &, const IO & io, Expr * );

namespace builtin
{

Expr * add( void * param, Context & context, const IO & io, Expr * arg );

Expr * mult( void * param, Context & context, const IO & io, Expr * arg );

} // namespace builtin

} // namespace lisp