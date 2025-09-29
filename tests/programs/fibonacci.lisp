(defun fibonacci (n)
  (if (< n 2)
    n
    (+ (fibonacci (- n 1))
       (fibonacci (- n 2)))))

(defun range (start end)
  (if (>= start end)
    nil
    (cons start (range (+ start 1) end))))

(define numbers (range 0 15))

(println (map fibonacci numbers))
