#include <gtest/gtest.h>

#include "lisp.h"

using namespace lisp;

class LispTest : public ::testing::Test
{
 public:
   LispTest()
       : io( out, err )
   {
      ctx.load_runtime();
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

TEST_F( LispTest, test_01 )
{
   Expr * exp = make_number( 2.5 );

   eval_print( exp, ctx, io );

   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "2.5" );
}

TEST_F( LispTest, test_04 )
{

   Expr * exp = make_nil();

   eval_print( exp, ctx, io );

   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "nil" );
}

TEST_F( LispTest, test_02 )
{

   Expr * exp
       = make_cons( make_symbol( "+" ), make_cons( make_number( 2 ), make_cons( make_number( 3 ), make_nil() ) ) );

   eval_print( exp, ctx, io );

   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "5" );
}
