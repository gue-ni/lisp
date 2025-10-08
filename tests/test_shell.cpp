#include <gtest/gtest.h>

#include "util.h"

using namespace lisp;

class ShellTest : public LispTest {
  public:
void SetUp() override
  {
  load_shell_macros(ctx,io);

  }

  void TearDown() override
  {
  }
};

#ifdef __linux__
TEST_F( ShellTest, test_shell_01 )
{
  std::string src = R"(
(defvar result
   ($
      (pipe
         (sh find "src")
         (sh grep "lisp.h"))))

(print result)
   )";
  int r           = eval( src, ctx, io );
  EXPECT_EQ( err.str(), "" );
  EXPECT_EQ( out.str(), "src/lisp.h" );
}

TEST_F( ShellTest, test_shell_02 )
{
  std::string src = R"(
(defvar reversed
   ($
      (pipe
         (<<< "hello world")
         (sh rev))))

(print reversed)
   )";
  int r           = eval( src, ctx, io );
  EXPECT_EQ( err.str(), "" );
  EXPECT_EQ( out.str(), "dlrow olleh" );
}

TEST_F( ShellTest, test_shell_03 )
{
  std::string src = R"(
(print ($ (sh uname -o)))
   )";
  int r           = eval( src, ctx, io );
  EXPECT_EQ( err.str(), "" );
  EXPECT_EQ( out.str(), "GNU/Linux" );
}

#endif
