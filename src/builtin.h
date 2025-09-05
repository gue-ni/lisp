#pragma once

#include "expr.h"
#include "util.h"

namespace lisp {

class Context;

typedef Expr *(*NativeFunction)(void *param, Context &context, const IO &io,
                                Expr *args);

namespace builtin {

Expr *print(void *param, Context &context, const IO &io, Expr *args);

Expr *add(void *param, Context &context, const IO &io, Expr *args);

} // namespace builtin

} // namespace lisp