# Lisp Interpreter

Lisp is one of the oldest high-level programming languages, dating back to the
late 1950s. It is characterized by it's use of parenthesis, prefix notation
and S-expressions, which represent both code and data. While Lisp has fallen
somewhat out of favour these days, it has inspired many languages

This is a small Lisp interpreter written for educational purposes.

## Features

-   Interactive REPL
-   Arithmetic, conditionals and list manipulation
-   Functions and lambdas
-   Simple macros

```lisp
; recursive functions
(defun factorial (n)
    (if (= n 0)
        1
        (* n (factorial (- n 1)))))

(factorial 5) ; => 120
```

```lisp
; clojures
(defun make-adder (a)
  (lambda (b) (+ a b)))

(define add5 (make-adder 5))

(add5 3) ; => 8
```

I have written a simple function for dumping the internal datastructure as json, which is very useful for debugging.

The expression `(+ 2 3)` would look like this:

```json
{
	"car": "symbol(+)",
	"cdr": {
		"car": 2,
		"cdr": {
			"car": 3,
			"cdr": null
		}
	}
}
```

## Documentation

Lisp programs are built from S-expressions, which are either _atoms_ (numbers,
symbols or strings) or _lists_ enclosed in parantheses. Functions and operations
are written in prefix notation, where the operator comes first, followed by its
arguments.

```lisp
(fn arg1 arg2 arg3)
```

For example, `(+ 2 3 (* 4 5))` is equal to `2 + 3 + 4 * 5` and `(fn 1 2)`
is equal to something like `fn(1, 2)` in a language like python.

### Special Forms

#### `quote`

Prevent evaluation of an expression, returning it literally.

```lisp
(quote (1 2 3)) ; => (1 2 3)

; shorthand:
'(1 2 3) ; => (1 2 3)
```

#### `list`

Create a list of elements.

```lisp
(list 1 2 3)
; =>  (1 2 3)
```

#### `let`

Binds variables locally.

```lisp
(let ((x 2) (y 3))
    (+ x y))
; => 5
```

#### `progn`

Evaluates multiple expressions in order, return result of the last.

```lisp
(prog
    (print "Hello ")
    (print "World ")
    (+ 2 3))
; => Hello World 5
```

#### `if`

Conditional evaluation.

```lisp
(if (= 2 3)
    "equal"
    "not equal")
; => "not equal"
```

#### `cond`

Conditional evaluation, tests clauses in order and executes the first one that is true.

```lisp
(define x 5)
(cond
    ((> x 5) "greater")
    ((= x 5) "equal")
    (true "lower"))     ; fallback
; => "equal"
```

#### `define`

Define a variable.

```lisp
(define my-var 42)
(println my-var)
; => 42
```

#### `lambda`

Create an anonymous function.

```lisp
(lambda (x) (* x x))
```

#### `defun`

Define a named function.

```lisp
(defun add (a b)
    (+ a b))
```

### Builtin Functions

| Keyword                       | Purpose               | Example                   |
| ----------------------------- | --------------------- | ------------------------- |
| `+`, `-`, `*`, `/`            | Arithemtic operations | `(* 2 3)`                 |
| `=`, `<`, `>`, `<=`, `>=`     | Logic operations      |                           |
| `print` , `println`           |                       |                           |
| `car`, `cdr`                  |                       |                           |
| `cons`                        |                       |                           |
| `append`                      |                       |                           |
| `map`                         |                       |                           |
| `filter`                      |                       |                           |
| `length`                      |                       |                           |
| `read`                        |                       |                           |
| `eval`                        |                       |                           |
| `null?`, `number?`, `string?` | Check for type        | `(string? "hello")`       |
| `load`                        | Import file           | `(load "my-module.lisp")` |
| `str`                         |                       |                           |
