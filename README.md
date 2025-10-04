# A Lisp Interpreter for Shell Scripting

This project is a lightweight Lisp interpreter written in C++ with a focus on
shell scripting. It lets you combine the expressive power of Lisp with the
practicality of the Unix shell: you can run commands, capture output, pipe
between processes, and still use Lisp syntax for logic and structure. Think of
it as writing your shell scripts in Lisp instead of Bash.

## How to install

```bash
bash <(curl -s https://www.jakobmaier.at/files/lisp/release/install-lisp.sh)
```

## Features

- [x] Interactive REPL
- [x] Arithmetic, conditionals and list manipulation
- [x] Garbage Collection
- [x] Functions and lambdas
- [x] Macros
- [x] Shell Scripting

## Examples

```lisp
; recursive functions
(defun factorial (n)
    (if (= n 0)
        1
        (* n (factorial (- n 1)))))

(factorial 5) ; => 120
```

```lisp
; closures
(defun make-adder (a)
  (lambda (b) (+ a b)))

(defvar add5 (make-adder 5))

(add5 3) ; => 8
```

## Documentation

- [Gettings Started](doc/getting-started.md)
- [Shell Scripting](doc/shell-scripting.md)