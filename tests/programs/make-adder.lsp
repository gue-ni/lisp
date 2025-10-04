(defvar make-adder
  (lambda (x)
    (lambda (y) (+ x y))))

(defvar add-5 (make-adder 5))

(println (add-5 3))
