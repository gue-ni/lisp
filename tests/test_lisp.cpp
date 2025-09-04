#include <gtest/gtest.h>

#include "lisp.h"

using namespace lisp;

TEST(lisp, test_01) {

  Context ctx;
  std::ostringstream out, err;
  IO io(out, err);

  Expr exp(2.5);

  print(eval(&exp, ctx, io), io);

  EXPECT_EQ(out.str(), "2.5");
}

TEST(lisp, test_02) {

  Context ctx;
  std::ostringstream out, err;
  IO io(out, err);

  Expr *exp = make_list(make_number(5), make_list(make_number(2), make_nil()));

  print(eval(exp, ctx, io), io);

  EXPECT_EQ(out.str(), "2.5");
}