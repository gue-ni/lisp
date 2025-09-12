(define make-adder
  (lambda (x)
    (lambda (y) (+ x y))))

(define add-5 (make-adder 5))

(add-5 3)
