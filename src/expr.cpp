#include "expr.h"

namespace lisp
{

Expr * NativeFn::operator()( Expr * args, Context & context, const IO & io )
{
   return fn( nullptr, context, io, args );
}

} // namespace lisp