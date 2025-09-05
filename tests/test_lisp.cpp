#include <gtest/gtest.h>

#include "lisp.h"

using namespace lisp;

TEST( lisp, test_01 )
{

   Context ctx;
   std::ostringstream out, err;
   IO io( out, err );

   Expr exp( 2.5 );

   eval_print( &exp, ctx, io );

   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "2.5" );
}

TEST( lisp, test_02 )
{

   Context ctx;
   std::ostringstream out, err;
   IO io( out, err );

   ctx.load_runtime();

   Expr * exp
       = make_cons( make_symbol( "+" ), make_cons( make_number( 2 ), make_cons( make_number( 5 ), make_nil() ) ) );

   print_expr( exp, io );

   eval_print( exp, ctx, io );

   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "" );
}

TEST( lisp, test_03 )
{

   Context ctx;
   std::ostringstream out, err;
   IO io( out, err );

   std::string src = "(+ 2 3)";

   ( void ) eval( src, ctx, io );

   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "5" );
}
