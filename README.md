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

(defvar add5 (make-adder 5))

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

- [Gettings Started](doc/getting-started.md)
- [Shell Scripting](doc/shell-scripting.md)