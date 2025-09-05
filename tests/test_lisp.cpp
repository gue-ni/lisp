#include <gtest/gtest.h>

#include "lisp.h"
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
   Expr * exp = make_number( 2.5 );

   eval_print( exp, ctx, io );

   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "2.5" );
}

TEST_F( LispTest, test_eval_nil_01 )
{
   Expr * exp = make_nil();

   eval_print( exp, ctx, io );

   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "nil" );
}

TEST_F( LispTest, test_eval_string_01 )
{
   Expr * exp = make_string( "Hello, LISP!" );

   eval_print( exp, ctx, io );

   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "\"Hello, LISP!\"" );
}

TEST_F( LispTest, test_eval_symbol_01 )
{
   Expr * exp = make_symbol( "+" );

   eval_print( exp, ctx, io );

   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "<native-fn>" );
}

TEST_F( LispTest, test_eval_non_existing_symbol_01 )
{
   Expr * exp = make_symbol( "does-not-exist" );

   eval_print( exp, ctx, io );

   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "nil" );
}

TEST_F( LispTest, test_eval_exp_with_native_fn_add_01 )
{
   Expr * exp
       = make_cons( make_symbol( "+" ), make_cons( make_number( 2 ), make_cons( make_number( 3 ), make_nil() ) ) );

   eval_print( exp, ctx, io );

   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "5" );
}

TEST_F( LispTest, test_eval_exp_with_native_fn_mult_01 )
{
   Expr * exp
       = make_cons( make_symbol( "*" ), make_cons( make_number( 2 ), make_cons( make_number( 3 ), make_nil() ) ) );

   eval_print( exp, ctx, io );

   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "6" );
}

TEST_F( LispTest, test_tokenizer_01 )
{
   {
      std::string source = "(+ 3.14 5)";
      auto tokens        = tokenize( source );
      EXPECT_EQ( tokens.size(), 6 );
   }
   {
      std::string source = "(lambda (arg) (+ arg 1))";
      auto tokens        = tokenize( source );
   }
   {
      std::string source = "123";
      auto tokens        = tokenize( source );
   }
   {
      std::string source = "abc";
      auto tokens        = tokenize( source );
   }
   {
      std::string source = "(+ 2 (* 3 4))";
      auto tokens        = tokenize( source );
   }
}

TEST_F( LispTest, test_parser_01 )
{
   {
      // std::string source = "5";
      // Expr* expr = parse(source);
   }
}
