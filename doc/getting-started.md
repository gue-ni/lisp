# Getting Started

## Content

- [Getting Started](#getting-started)
  - [Content](#content)
  - [Basics](#basics)
  - [Literals](#literals)
  - [Special Forms](#special-forms)
    - [`quote`](#quote)
    - [`list`](#list)
    - [`let`](#let)
    - [`progn`](#progn)
    - [`if`](#if)
    - [`cond`](#cond)
    - [`defvar`](#defvar)
    - [`lambda`](#lambda)
    - [`defun`](#defun)
    - [`defmacro`](#defmacro)
  - [Builtin Functions](#builtin-functions)

## Basics

Lisp programs are built from S-expressions, which are either _atoms_ (numbers,
symbols or strings) or _lists_ enclosed in parantheses. Functions and operations
are written in prefix notation, where the operator comes first, followed by the
arguments.

```lisp
(fn arg1 arg2 arg3)
```

For example, `(+ 2 3 (* 4 5))` is equal to `2 + 3 + 4 * 5` and `(fn 1 2)`
is equal to something like `fn(1, 2)` in a language like python.

## Literals

_Lisp_ supports the usual basic data types, such as _nil_, _boolean_, _real_
(floating point numbers), _integer_ and _string_.

```lisp
; nil
nil

; boolean literals
; => true
true
t

; => false
false
f

; floating point literal
2.5

; integer literal
-10

; string literals
"my string"

; list literal
(list "hello" 2 3 t)
; => ("hello" 2 3 true)
```


## Special Forms

### `quote`

Prevent evaluation of an expression, returning it literally.

```lisp
(quote (1 2 3)) ; => (1 2 3)

; shorthand:
'(1 2 3) ; => (1 2 3)
```

### `list`

Create a list of elements.

```lisp
(list 1 2 3)
; =>  (1 2 3)
```

### `let`

Binds variables locally.

```lisp
(let ((x 2) (y 3))
    (+ x y))
; => 5
```

### `progn`

Evaluates multiple expressions in order, return result of the last.

```lisp
(prog
    (print "Hello ")
    (print "World ")
    (+ 2 3))
; => Hello World 5
```

### `if`

Conditional evaluation.

```lisp
(if (= 2 3)
    "equal"
    "not equal")
; => "not equal"
```

### `cond`

Conditional evaluation, tests clauses in order and executes the first one that
is true.

```lisp
(defvar x 5)
(cond
    ((> x 5) "greater")
    ((= x 5) "equal")
    (true "lower"))     ; fallback
; => "equal"
```

### `defvar`

Define a variable.

```lisp
(defvar my-var 42)
(println my-var)
; => 42
```

### `lambda`

Create an anonymous function.

```lisp
(lambda (x) (* x x))
```

### `defun`

Define a named function.

```lisp
(defun add (a b)
    (+ a b))

; defun also supports '&rest' to capture a list of arguments
(defun my-fn (a b &rest c)
  (list a b c))

(my-fn 1 2 3 4 5)
; => (1 2 (3 4 5))
```

### `defmacro`

Define a macro. Unlike a function, the arguments are not evaluated before
passing them to the macro. Arguments can be unquoted with `,` and `,@`.

```lisp
; macro to test if a symbol is defined.
(defmacro symbolp (s)
  `(let ((e ,s)) (not (error? e))))
```

## Builtin Functions

| Keyword                                 | Purpose                                       | Example                                      |
| --------------------------------------- | --------------------------------------------- | -------------------------------------------- |
| `+`, `-`, `*`, `/`                      | Arithemtic operations                         | `(* 2 3)`                                    |
| `=`, `<`, `>`, `<=`, `>=`               | Logic operations                              | `(= 2 3)`, `(< 2 3)`                         |
| `and` , `or`, `not`                     | Boolean operators                             |                                              |
| `print` , `println`                     | Print to command line                         |                                              |
| `car`                                   | Access _car_ of _cons_ cell                   |                                              |
| `cdr`                                   | Access _cdr_ of _cons_ cell                   |                                              |
| `cons`                                  | Create cons cell                              |                                              |
| `append`                                | Concatinate lists                             |                                              |
| `map`                                   | Return list of elements with function applied | `(map (lambda (x) (* x x)) (list 1 2 3))`    |
| `filter` `(filter pred lst)`            | Return list of elements for wich pred is true | `(filter (lambda (x) (> x 1)) (list 1 2 3))` |
| `apply`                                 | Apply function to list of arguments           | `(apply + (list 1 2 3))`                     |
| `length`                                | Get length of list                            |                                              |
| `read`                                  | Convert string to lisp object                 |                                              |
| `eval`                                  | Eval lisp object                              |                                              |
| `null?`, `number?`, `string?`, `error?` | Check for type                                | `(string? "hello")`                          |
| `load`                                  | Import file                                   | `(load "my-module.lsp")`                     |
| `str`                                   | Convert expression to string                  |                                              |
| `strcat`                                | Concatinate strings                           |                                              |
| `strcmp`                                | Compare string                                |                                              |
| `strlen`                                | Get length of string                          |                                              |
| `strip`                                 | Remove leading or trailing whitespace         |                                              |
