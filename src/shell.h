#pragma once

#include "expr.h"

namespace lisp {

namespace shell {

Expr *f_exec(Expr *arg, Context &context, const IO &io);

Expr *f_make_pipe(Expr *arg, Context &context, const IO &io);

Expr *f_pipe(Expr *arg, Context &context, const IO &io);

} // namespace shell

} // namespace lisp