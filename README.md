# Lisp

Lisp is one of the oldest programming languages, first developed in the 1950s.
It is characterized by it's use of parenthesis, prefix notation and
s-expressions, which represent both code and data.

This is my attempt at writing a Lisp interpreter.

The fundamental data structure in lisp is the cons cell. The cons cell is a
struct with two values, car (first element) and cdr (the rest of the list). This
can be used to create recursive linked lists, which are used to represent the a
expressions.

I have created a simple function to dump such a expression as familiar json
data. The expression `(+ 2 3)` thus looks like this in the internal representation:

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

## Example

```lisp
(defun fib (n)
    (if (< n 2)
        n
        (+ (fib (- n 1))
           (fib (- n 2)))))

(fib 6)
```

