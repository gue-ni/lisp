#pragma once

#include "expr.h"
#include "tokenizer.h"

namespace lisp
{

class Parser
{
 public:
   Parser( const Tokens & tokens );

 private:
};

Expr * parse( const Tokens & tokens );

}; // namespace lisp