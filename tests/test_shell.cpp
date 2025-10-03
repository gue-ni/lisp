#include <gtest/gtest.h>

#include "util.h"

using namespace lisp;

#ifdef __linux__
TEST_F( LispTest, test_shell_01 )
{
   std::string src = R"(
(load "stdlib/shell.lsp")

(defvar result
   (from-stream
      (pipe
         (sh find "src")
         (sh grep "lisp.h"))))

(print result)
   )";
   int r           = eval( src, ctx, io );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "src/lisp.h\n" );
}

TEST_F( LispTest, test_shell_02 )
{
   std::string src = R"(
(load "stdlib/shell.lsp")

(defvar reversed
   (from-stream
      (pipe
         (to-stream "hello world")
         (sh rev))))

(print reversed)
   )";
   int r           = eval( src, ctx, io );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "dlrow olleh" );
}

TEST_F( LispTest, test_shell_03 )
{
   std::string src = R"(
(load "stdlib/shell.lsp")

(print (from-stream (sh uname -o)))
   )";
   int r           = eval( src, ctx, io );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "GNU/Linux\n" );
}

#endif
