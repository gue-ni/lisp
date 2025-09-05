#include "builtin.h"
#include "expr.h"

namespace lisp {

namespace builtin {

Expr *add(void *param, Context &context, const IO &io, Expr *args) {

  return make_nil();
}
} // namespace builtin

} // namespace lisp