# Redstart - A Lisp Interpreter for Shell Scripting

![build workflow status](https://github.com/gue-ni/lisp/actions/workflows/build.yml/badge.svg)

Redstart (named after the [bird](https://en.wikipedia.org/wiki/Common_redstart)) is
a lightweight Lisp interpreter written in C++ with a focus on shell scripting.
It lets you combine the expressive power of Lisp with the practicality of the
Unix shell: you can run commands, capture output, pipe between processes, and
still use Lisp syntax for logic and structure. Think of it as writing your shell
scripts in Lisp instead of Bash.

## How to install

```bash
bash <(curl -s https://raw.githubusercontent.com/gue-ni/redstart/refs/heads/master/tools/install.sh)
```

## Quickstart

```bash
# Start the REPL
rst

# Run a script
rst my_script.lsp
```

## Examples

```lisp
; pipe output of 'ls' to 'grep'
(pipe (sh ls -la) (sh grep "*.txt"))

; save the output of 'cat' to variable 'content'
(defvar content ($ (sh cat "my-file.txt")))

; pipe the output of the function 'hello' into the 'rev' command
(defun hello (name)
  (strcat "Hello " name "!"))

(pipe (<<< (hello "Jakob")) (sh rev))
; => !bokaJ olleH

; eqivalent to 'sudo apt update && sudo apt upgrade -y' in bash
(and (sh sudo apt update) (sh sudo apt upgrade -y))

; define a function that uses 'scp' to upload some files to a server
(defun upload (source)
  (let ((user "root")
        (host "example.com")
        (target-dir "/var/www/files/")
        (target (strcat user "@" host ":" target-dir)))
    (sh scp source target)))

; execute 'ls' command, capture result
($ (sh ls))

; use _ to access result of last expression, split on newline
(defvar files (split "\n" _))

; upload all files
(for-each upload files)
```

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

- [Getting Started](doc/getting-started.md)
- [Shell Scripting](doc/shell-scripting.md)