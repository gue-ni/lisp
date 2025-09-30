#pragma once

#include "expr.h"

// how can i manage pipes?
// when calling exec, i need to have the pipes ready

namespace lisp {

namespace shell {

Expr *f_exec(Expr *arg, Context &context, const IO &io);

Expr *f_exec_piped(Expr *arg, Context &context, const IO &io);

Expr *f_make_pipe(Expr *arg, Context &context, const IO &io);

Expr *f_pipe(Expr *arg, Context &context, const IO &io);

} // namespace shell

} // namespace lisp