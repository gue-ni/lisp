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

TEST_F( LispTest, test_parse_01 )
{
   std::string src = "(+ 1 2";
   eval( src, ctx, io );
   EXPECT_EQ( err.str(),  "(error \"missing-parenthesis\")\n");
   EXPECT_EQ( out.str(), "" );
}

TEST_F( LispTest, test_parse_02 )
{
   std::string src = "(+ 1 2 (* 3 4 )";
   eval( src, ctx, io );
   EXPECT_EQ( err.str(),  "(error \"missing-parenthesis\")\n" );
   EXPECT_EQ( out.str(), "" );
}

TEST_F( LispTest, test_parse_03 )
{
   std::string src = "(+ 1 2))";
   eval( src, ctx, io );
   EXPECT_EQ( err.str(), "(error \"unexpected-parenthesis\")\n" );
   EXPECT_EQ( out.str(), "" );
}

TEST_F( LispTest, test_eval_number_01 )
{
   eval( "2.5", ctx, io );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "2.5" );
}

TEST_F( LispTest, test_eval_symbol_01 )
{
   eval( "+", ctx, io );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "(native-fn)" );
}

TEST_F( LispTest, test_eval_non_existing_symbol_01 )
{
   eval( "does-not-exist", ctx, io );
   EXPECT_EQ( err.str(), "(error \"undefined symbol 'does-not-exist'\")\n");
   EXPECT_EQ( out.str(), "" );
}

TEST_F( LispTest, test_nil_01 )
{
   eval( "'()", ctx, io );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "nil" );
}

TEST_F( LispTest, test_nil_02 )
{
   eval( "nil", ctx, io );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "nil" );
}

TEST_F( LispTest, test_add_01 )
{
   eval( "(+ 2 3)", ctx, io );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "5" );
}

TEST_F( LispTest, test_mult_01 )
{
   eval( "(* 2 3)", ctx, io );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "6" );
}

TEST_F( LispTest, test_div_01 )
{
   eval( "(/ 5 2)", ctx, io );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "2.5" );
}

TEST_F( LispTest, test_div_02 )
{
   eval( "(/ 5 0)", ctx, io );
   EXPECT_EQ( err.str(), "(error \"division by zero\")\n" );
   EXPECT_EQ( out.str(), "" );
}

TEST_F( LispTest, test_sub_01 )
{
   eval( "(- 5 3)", ctx, io );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "2" );
}

TEST_F( LispTest, test_sub_02 )
{
   eval( "(- 3 2 1)", ctx, io );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "0" );
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
   EXPECT_EQ( out.str(), "(lambda-fn)" );
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

TEST_F( LispTest, test_car_02 )
{
   std::string src = "(car (cons 1 2))";
   int r           = eval( src, ctx, io );
   EXPECT_EQ( r, 0 );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "1" );
}

TEST_F( LispTest, test_car_03 )
{
   std::string src = "(car '(1 2 3))";
   int r           = eval( src, ctx, io );
   EXPECT_EQ( r, 0 );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "1" );
}

TEST_F( LispTest, test_car_04 )
{
   std::string src = "(define x '(1 2 3)) (car x)";
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

TEST_F( LispTest, test_cdr_02 )
{
   std::string src = "(cdr (cons 1 2))";
   int r           = eval( src, ctx, io );
   EXPECT_EQ( r, 0 );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "2" );
}

TEST_F( LispTest, test_cdr_03 )
{
   std::string src = "(cdr '(1 2 3))";
   int r           = eval( src, ctx, io );
   EXPECT_EQ( r, 0 );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "(2 3)" );
}

TEST_F( LispTest, test_cdr_04 )
{
   std::string src = "(define x '(1 2 3)) (cdr x)";
   int r           = eval( src, ctx, io );
   EXPECT_EQ( r, 0 );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "(2 3)" );
}

TEST_F( LispTest, test_if_01 )
{
   std::string src = "(if true 1 2)";
   int r           = eval( src, ctx, io );
   EXPECT_EQ( r, 0 );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "1" );
}

TEST_F( LispTest, test_if_02 )
{
   std::string src = "(if false 1 2)";
   int r           = eval( src, ctx, io );
   EXPECT_EQ( r, 0 );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "2" );
}

TEST_F( LispTest, test_eq_01 )
{
   std::string src = "(= 42 42)";
   int r           = eval( src, ctx, io );
   EXPECT_EQ( r, 0 );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "true" );
}

TEST_F( LispTest, test_eq_02 )
{
   std::string src = "(= 42 68)";
   int r           = eval( src, ctx, io );
   EXPECT_EQ( r, 0 );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "false" );
}

TEST_F( LispTest, test_eq_03 )
{
   std::string src = "(= 5 (+ 2 3))";
   int r           = eval( src, ctx, io );
   EXPECT_EQ( r, 0 );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "true" );
}

TEST_F( LispTest, test_eq_04 )
{
   std::string src = "(= 1 1 1)";
   int r           = eval( src, ctx, io );
   EXPECT_EQ( r, 0 );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "true" );
}

TEST_F( LispTest, test_eq_05 )
{
   std::string src = "(= 1 2 3)";
   int r           = eval( src, ctx, io );
   EXPECT_EQ( r, 0 );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "false" );
}

TEST_F( LispTest, test_gt_01 )
{
   std::string src = "(> 3 2)";
   int r           = eval( src, ctx, io );
   EXPECT_EQ( r, 0 );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "true" );
}

TEST_F( LispTest, test_gt_02 )
{
   std::string src = "(> 2 3)";
   int r           = eval( src, ctx, io );
   EXPECT_EQ( r, 0 );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "false" );
}

TEST_F( LispTest, test_string_01 )
{
   std::string src = "\"hello world\"";
   int r           = eval( src, ctx, io );
   EXPECT_EQ( r, 0 );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "\"hello world\"" );
}

TEST_F( LispTest, test_read_eval_01 )
{
   std::string src = "(eval (read \"+ 1 2\"))";
   int r           = eval( src, ctx, io );
   EXPECT_EQ( r, 0 );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "3" );
}

#if 0
#if WIN32
TEST_F( LispTest, test_stdlib_02 )
{
   std::string src = "(import \"C:/Users/jakob/Documents/Projects/lisp/tests/examples/prog_002.lisp\") (area 5)";
   int r           = eval( src, ctx, io );
   EXPECT_EQ( r, 0 );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "78.5398" );
}
#endif
#endif

TEST_F( LispTest, test_lambda_05 )
{
   std::string src = R"(
(defun make-adder (a)
  (lambda (b) (+ a b)))

(define add5 (make-adder 5))

(add5 3)
  )";

   int r = eval( src, ctx, io );
   EXPECT_EQ( r, 0 );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "8" );
}

TEST_F( LispTest, test_lambda_06 )
{
   std::string src = R"(
(define pi 3.14159265359)

(define circle-area
  (lambda (r)
    (* pi (* r r))))

(circle-area 5)
   )";

   int r = eval( src, ctx, io );
   EXPECT_EQ( r, 0 );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "78.5398" );
}

TEST_F( LispTest, test_recursion_01 )
{
   std::string src = R"(
(defun factorial (n)
    (if (= n 0)
        1
        (* n (factorial (- n 1)))))

(factorial 5)
   )";

   int r = eval( src, ctx, io );
   EXPECT_EQ( r, 0 );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "120" );
}

TEST_F( LispTest, test_recursion_02 )
{
   std::string src = R"(
(defun fib (n)
    (if (< n 2)
        n
        (+ (fib (- n 1))
           (fib (- n 2)))))

(fib 6)
   )";

   int r = eval( src, ctx, io );
   EXPECT_EQ( r, 0 );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "8" );
}

TEST_F( LispTest, test_define_02 )
{
   std::string src = R"(
    (defun add (x y)
      (+ x y))

    (add 5 4)
    )";

   int r = eval( src, ctx, io );
   EXPECT_EQ( r, 0 );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "9" );
}

TEST_F( LispTest, test_map_01 )
{
   std::string src = R"(
(defun my-map (fn lst)
  (if (null? lst)
    '()
    (cons
      (fn (car lst))
      (my-map fn (cdr lst)))))

(my-map (lambda (n) (* n n)) '(1 2 3 4))
    )";

   int r = eval( src, ctx, io );
   EXPECT_EQ( r, 0 );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "(1 4 9 16)" );
}

TEST_F( LispTest, test_filter_01 )
{
   std::string src = R"(
(defun my-filter (pred lst)
  (if (null? lst)
    '()
    (if (pred (car lst))
      (cons (car lst) (my-filter pred (cdr lst)))
      (my-filter pred (cdr lst)))))

(my-filter (lambda (x) (> x 3)) '(1 2 3 4 5))
   )";

   int r = eval( src, ctx, io );
   EXPECT_EQ( r, 0 );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "(4 5)" );
}

TEST_F( LispTest, test_reduce_01 )
{
   std::string src = R"(
(defun my-reduce (f init lst)
  (if (null? lst)
      init
      (my-reduce f (f init (car lst)) (cdr lst))))

(my-reduce + 0 '(1 2 3 4 5))
   )";

   int r = eval( src, ctx, io );
   EXPECT_EQ( r, 0 );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "15" );
}

TEST_F( LispTest, test_range_01 )
{
   std::string src = R"(
(defun my-range (start end)
  (if (>= start end)
      '()
      (cons start (my-range (+ start 1) end))))

(my-range 2 6)
   )";

   int r = eval( src, ctx, io );
   EXPECT_EQ( r, 0 );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "(2 3 4 5)" );
}

TEST_F( LispTest, test_append_01 )
{
   std::string src = R"(
(append '(1 2 3 4) '(5 6 7 8))
   )";

   int r = eval( src, ctx, io );
   EXPECT_EQ( r, 0 );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "(1 2 3 4 5 6 7 8)" );
}

TEST_F( LispTest, test_length_01 )
{
   std::string src = R"(
(length '(1 2 3 4))
   )";

   int r = eval( src, ctx, io );
   EXPECT_EQ( r, 0 );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "4" );
}

TEST_F( LispTest, test_length_02 )
{
   std::string src = R"(
(length '())
   )";

   int r = eval( src, ctx, io );
   EXPECT_EQ( r, 0 );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "0" );
}

TEST_F( LispTest, test_defun_01 )
{
   std::string src = R"(
(defun add (a b) (+ a b))

(add 2 3)
   )";
   int r           = eval( src, ctx, io );
   EXPECT_EQ( r, 0 );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "5" );
}

TEST_F( LispTest, test_unquote_01 )
{
   std::string src = R"(
`(1 ,(+ 2 3) 4)
   )";

   int r = eval( src, ctx, io );
   EXPECT_EQ( r, 0 );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "(1 5 4)" );
}

TEST_F( LispTest, test_macro_01 )
{

   std::string src = R"(

(defmacro my-macro (c)
  `(if ,c "case 1" "case 2"))

(my-macro (= 2 3))
   )";

   int r = eval( src, ctx, io );
   EXPECT_EQ( r, 0 );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "\"case 2\"" );
}

TEST_F( LispTest, test_macro_02 )
{

   std::string src = R"(

(defmacro my-macro (t c1 c2)
  `(if ,t c1 c2))

(print (my-macro (= 2 3) "c-1" "c-2"))

   )";

   int r = eval( src, ctx, io );
   EXPECT_EQ( r, 0 );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "c-2" );
}

#if 0
TEST_F( LispTest, test_unquote_02 )
{
   std::string src = R"(
(defmacro unless (test form)
  `(if (not ,test)
      ,form
      "wrong"))


(unless (= 2 2) "hello")
   )";
   int r           = eval( src, ctx, io );
   EXPECT_EQ( r, 0 );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "\"hello\"" );
}
#endif

TEST_F( LispTest, test_unquote_splice_01 )
{
   std::string src = R"(
`(1 ,@'(2 3) 4)
   )";
   int r           = eval( src, ctx, io );
   EXPECT_EQ( r, 0 );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "(1 2 3 4)" );
}

TEST_F( LispTest, test_rest_argument_01 )
{
   std::string src = R"(
(defun my-fn (a b & rest)
  rest)

(my-fn 1 2 3 4)
   )";

   int r = eval( src, ctx, io );

   EXPECT_EQ( r, 0 );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "(3 4)" );
}

TEST_F( LispTest, test_negative_number_01 )
{
   std::string src = R"(
(+ -4.5 3)
   )";

   int r = eval( src, ctx, io );

   EXPECT_EQ( r, 0 );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "-1.5" );
}

TEST_F( LispTest, test_do_01 )
{
   std::string src = "(progn (print 1) (print 2) (* 4 5))";

   int r = eval( src, ctx, io );
   EXPECT_EQ( r, 0 );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "1220" );
}

TEST_F( LispTest, test_let_01 )
{
   std::string src = "(let ((x 2) (y 3)) (* x y))";

   int r = eval( src, ctx, io );
   EXPECT_EQ( r, 0 );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "6" );
}

TEST_F( LispTest, test_let_02 )
{
   std::string src = R"(
(let ((x 5)
     (y (* 2 2)))
     (+ x y))
   )";

   int r = eval( src, ctx, io );
   EXPECT_EQ( r, 0 );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "9" );
}

TEST_F( LispTest, test_let_03 )
{
   std::string src = "(let ((x 10)) (* 2 x))";

   int r = eval( src, ctx, io );
   EXPECT_EQ( r, 0 );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "20" );
}

TEST_F( LispTest, test_let_04 )
{
   std::string src = R"(
(let ((x 5)
     (y (let ((x 10)) (* 2 x))))
     (+ x y))
   )";

   int r = eval( src, ctx, io );
   EXPECT_EQ( r, 0 );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "25" );
}

TEST_F( LispTest, test_let_05 )
{
   std::string src = "(let ((a 10) (b (* 2 a))) (+ a b))";

   int r = eval( src, ctx, io );
   EXPECT_EQ( r, 0 );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "30" );
}

TEST_F( LispTest, test_print_01 )
{
   std::string src = "(print 1 2 3)";

   int r = eval( src, ctx, io );
   EXPECT_EQ( r, 0 );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "123" );
}

TEST_F( LispTest, test_str_01 )
{
   std::string src = "(print (str \"hello \" \"world\"))";

   int r = eval( src, ctx, io );
   EXPECT_EQ( r, 0 );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "hello world" );
}

TEST_F( LispTest, test_map_02 )
{
   std::string src = "(map (lambda (n) (* 2 n)) (list 1 2 3 4))";

   int r = eval( src, ctx, io );
   EXPECT_EQ( r, 0 );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "(2 4 6 8)" );
}

TEST_F( LispTest, test_map_03 )
{
   std::string src = R"(
(defun square (n)
  (* n n))

(map square (list 1 2 3 4))
)";

   int r = eval( src, ctx, io );
   EXPECT_EQ( r, 0 );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "(1 4 9 16)" );
}

TEST_F( LispTest, test_filter_02 )
{
   std::string src = "(filter (lambda (n) (> n 2)) (list 1 2 3 4 5))";

   int r = eval( src, ctx, io );
   EXPECT_EQ( r, 0 );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "(3 4 5)" );
}

TEST_F( LispTest, test_filter_03 )
{
   std::string src = "(filter number? (list 1 \"two\" 3 \"four\"))";
   int r           = eval( src, ctx, io );
   EXPECT_EQ( r, 0 );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "(1 3)" );
}

TEST_F( LispTest, test_back_inserter_01 )
{
   ListBuilder builder;
   builder.append( make_number( 1 ) );
   builder.append( make_number( 2 ) );
   builder.append( make_number( 3 ) );
   Expr * list = builder.list();

   EXPECT_EQ( to_string( list ), "(1 2 3)" );
}

TEST_F( LispTest, test_loop_01 )
{
   std::string src = R"(
(define my-list (list 1 2 3 4 5))

(progn
   (defun my-loop (l)
      (if (null? l)
         nil
         (progn
            (let ((el (car l))) (print el))
            (my-loop (cdr l)))))
   (my-loop my-list))
   )";

   int r = eval( src, ctx, io );
   EXPECT_EQ( r, 0 );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "12345nil" );
}

TEST_F( LispTest, test_macro_04 )
{
   std::string src = R"(
(define my-list (list 1 2 3))

(defmacro my-macro (lst)
   `(print ,lst))

(my-macro my-list)
   )";

   int r = eval( src, ctx, io );
   EXPECT_EQ( r, 0 );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "(1 2 3)" );
}

TEST_F( LispTest, test_macro_05 )
{
   std::string src = R"(
(define my-list (list 1 2 3))

(defmacro my-macro (lst)
   `(if (null? ,lst) 1 2))

(my-macro my-list)
   )";

   int r = eval( src, ctx, io );
   EXPECT_EQ( r, 0 );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "2" );
}

TEST_F( LispTest, test_comment_01 )
{
   std::string src = R"(
    ; some comment
    (+ 2 3)
   )";

   int r = eval( src, ctx, io );
   EXPECT_EQ( r, 0 );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "5" );
}

TEST_F( LispTest, test_comment_02 )
{
   std::string src = "(+ 2 3) ; inline comment";
   int r           = eval( src, ctx, io );
   EXPECT_EQ( r, 0 );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "5" );
}

TEST_F( LispTest, test_cond_01 )
{
   std::string src = R"(
(define x 5)

(print (cond
  ((> x 3) "option 1")
  ((= x 3) "option 2")
  (true    "option 3")))
   )";

   int r = eval( src, ctx, io );
   EXPECT_EQ( r, 0 );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "option 1" );
}

TEST_F( LispTest, test_cond_02 )
{
   std::string src = R"(
(define x 3)

(print (cond
  ((> x 3) "option 1")
  ((= x 3) "option 2")
  (true    "option 3")))
   )";

   int r = eval( src, ctx, io );
   EXPECT_EQ( r, 0 );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "option 2" );
}

TEST_F( LispTest, test_cond_03 )
{
   std::string src = R"(
(define x -1)

(print (cond
  ((> x 3) "option 1")
  ((= x 3) "option 2")
  (true    "option 3")))
   )";

   int r = eval( src, ctx, io );
   EXPECT_EQ( r, 0 );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "option 3" );
}

TEST_F( LispTest, test_logic_01 )
{
   std::string src = "(and true false)";
   int r           = eval( src, ctx, io );
   EXPECT_EQ( r, 0 );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "false" );
}

TEST_F( LispTest, test_logic_02 )
{
   std::string src = "(or true false)";
   int r           = eval( src, ctx, io );
   EXPECT_EQ( r, 0 );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "true" );
}

TEST_F( LispTest, test_logic_03 )
{
   std::string src = "(and (or nil 1) (or \"hello world\" false))";
   int r           = eval( src, ctx, io );
   EXPECT_EQ( r, 0 );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "true" );
}

TEST_F( LispTest, test_apply_1 )
{
   std::string src = "(apply + (list 1 2 3))";
   int r           = eval( src, ctx, io );
   EXPECT_EQ( r, 0 );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "6" );
}

#ifdef __linux__
TEST_F( LispTest, test_shell_01 )
{
   std::string src = R"(
(load "stdlib/shell.lsp")

(define result (pipe (sh find "src") (from-stream (sh grep "lisp.h"))))

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
(define reversed (from-stream (pipe (to-stream "hello world") (sh rev))))
(print reversed)
   )";
   int r           = eval( src, ctx, io );
   EXPECT_EQ( err.str(), "" );
   EXPECT_EQ( out.str(), "dlrow olleh" );
}

#endif


