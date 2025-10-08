#pragma once

#include <gtest/gtest.h>

#include "lisp.h"
#include "parser.h"
#include "tokenizer.h"

using namespace lisp;

class LispTest : public ::testing::Test
{
public:
  LispTest()
      : io( out, err )
  {
  }



protected:
  Context ctx;
  std::ostringstream out, err;
  IO io;
};