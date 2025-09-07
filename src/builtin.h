#pragma once

#include "util.h"

namespace lisp
{

struct Expr;

class Context;

typedef Expr * ( *NativeFunction )( Expr *, Context &, const IO & io );

namespace builtin
{

Expr * print( Expr * arg, Context & context, const IO & io );

Expr * plus( Expr * arg, Context & context, const IO & io );

Expr * minus( Expr * arg, Context & context, const IO & io );

Expr * mult( Expr * arg, Context & context, const IO & io );

Expr * div( Expr * arg, Context & context, const IO & io );

} // namespace builtin

} // namespace lisp