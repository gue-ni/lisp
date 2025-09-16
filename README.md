# Lisp

Lisp is one of the oldest programming languages, first developed in the 1950s. It is characterized by it's use of parenthesis, prefix notation and s-expressions, which represent both code and data.

This is my attempt at writing a Lisp interpreter.

## Example

```lisp
(define factorial
  (lambda (n)
    (if (= n 0)
        1
        (* n (factorial (- n 1))))))

(factorial 5)
```

