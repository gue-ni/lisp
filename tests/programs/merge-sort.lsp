; merge two lists
(defun merge (left right)
  (cond
    ((null? left) right)
    ((null? right) left)
    ((< (car left) (car right))
      (cons (car left) (merge (cdr left) right)))
    (true
      (cons (car right) (merge left (cdr right))))))

(defun slice-helper (start end lst idx)
  (cond
    ((null? lst) nil)
    ((>= idx end) nil)
    ((< idx start)
      (slice-helper start end (cdr lst) (+ idx 1)))
    (true
      (cons (car lst) (slice-helper start end (cdr lst) (+ idx 1))))))

; return sublist lst[start:end] (not inclusive)
(defun slice (start end lst)
  (slice-helper start end lst 0))

; sort a list of numbers
(defun merge-sort (lst)
  (cond
    ((null? lst) lst)
    ((null? (cdr lst)) lst)
    (true (let ((len (length lst))
            (half-len (/ len 2))
            (left (slice 0 half-len lst))
            (right (slice half-len len lst)))
        (merge (merge-sort left) (merge-sort right))))))

(println (merge-sort (list 42 7 89 56 13 78 31 64 5 90)))
