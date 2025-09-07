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

   void SetUp() override
   {
   }

   void TearDown() override
   {
   }

 protected:
   Context ctx;
   std::ostringstream out, err;
   IO io;
};

TEST_F( LispTest, test_eval_number_01 )
{
   eval( "2.5", ctx, io );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "2.5" );
}

TEST_F( LispTest, test_eval_nil_01 )
{
   Expr * exp = make_nil();
   eval( "'()", ctx, io );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "()" );
}

#if 0
TEST_F( LispTest, test_eval_string_01 )
{
   Expr * exp = make_string( "Hello, LISP!" );

   eval( exp, ctx, io );

   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "\"Hello, LISP!\"" );
}
#endif

TEST_F( LispTest, test_eval_symbol_01 )
{
   eval( "+", ctx, io );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "<native-fn>" );
}

TEST_F( LispTest, test_eval_non_existing_symbol_01 )
{
   eval( "does-not-exist", ctx, io );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "()" );
}

TEST_F( LispTest, test_eval_exp_with_native_fn_add_01 )
{
   eval( "(+ 2 3)", ctx, io );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "5" );
}

TEST_F( LispTest, test_eval_exp_with_native_fn_mult_01 )
{
   eval( "(* 2 3)", ctx, io );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "6" );
}

TEST_F( LispTest, test_eval_num_01 )
{
   std::string source = "3.1415";
   int r              = eval( source, ctx, io );
   EXPECT_EQ( r, 0 );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "3.1415" );
}

TEST_F( LispTest, test_eval_math_01 )
{
   int r = eval( "(+ 1 2 3)", ctx, io );
   EXPECT_EQ( r, 0 );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "6" );
}

TEST_F( LispTest, test_eval_math_02 )
{
   std::string source = "(+ 1 (* 2 3))";
   int r              = eval( source, ctx, io );
   EXPECT_EQ( r, 0 );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "7" );
}

TEST_F( LispTest, test_define_01 )
{
   std::string source = "(define x 42) (print x)";
   int r              = eval( source, ctx, io );
   EXPECT_EQ( r, 0 );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "42" );
}

TEST_F( LispTest, test_quote_01 )
{
   std::string source = "(quote (1 2 3))";
   int r              = eval( source, ctx, io );
   EXPECT_EQ( r, 0 );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "(1 2 3)" );
}

TEST_F( LispTest, test_quote_02 )
{
   std::string source = "'(1 2 3)";
   int r              = eval( source, ctx, io );
   EXPECT_EQ( r, 0 );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "(1 2 3)" );
}

TEST_F( LispTest, test_lambda_01 )
{
   std::string source = "(lambda (x) (* x 2))";
   int r              = eval( source, ctx, io );
   EXPECT_EQ( r, 0 );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "<lambda>" );
}

TEST_F( LispTest, test_lambda_02 )
{
   std::string source = "(define fn (lambda (x) (* x 2))) (fn 8)";
   int r              = eval( source, ctx, io );
   EXPECT_EQ( r, 0 );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "16" );
}

TEST_F( LispTest, test_lambda_03 )
{
   std::string source = "((lambda (x) (* x 2)) 5)";
   int r              = eval( source, ctx, io );
   EXPECT_EQ( r, 0 );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "10" );
}

TEST_F( LispTest, test_lambda_04 )
{
   std::string src = "(define add (lambda (x y) (+ x y))) (add 2 3)";
   int r           = eval( src, ctx, io );
   EXPECT_EQ( r, 0 );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "5" );
}

TEST_F( LispTest, test_cons_01 )
{
   std::string src = "(cons 1 2)";
   int r           = eval( src, ctx, io );
   EXPECT_EQ( r, 0 );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "(1 . 2)" );
}

TEST_F( LispTest, test_cons_02 )
{
   std::string src = "(cons 1 (cons 2 (cons 3 '())))";
   int r           = eval( src, ctx, io );
   EXPECT_EQ( r, 0 );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "(1 2 3)" );
}

TEST_F( LispTest, test_car_01 )
{
   std::string src = "(car (cons 1 2))";
   int r           = eval( src, ctx, io );
   EXPECT_EQ( r, 0 );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "1" );
}

TEST_F( LispTest, test_cdr_01 )
{
   std::string src = "(cdr (cons 1 2))";
   int r           = eval( src, ctx, io );
   EXPECT_EQ( r, 0 );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "2" );
}
