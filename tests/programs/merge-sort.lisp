; merge two lists
(defun merge (left right)
  (if (null? left)
    right
    (if (null? right)
      left
      (if (< (car left) (car right))
        (cons (car left) (merge (cdr left) right))
        (cons (car right) (merge left (cdr right)))))))

(defun slice-helper (start end lst idx)
  (if (null? lst)
    nil
    (if (>= idx end)
      nil
      (if (< idx start) ; skip elements before start
        (slice-helper start end (cdr lst) (+ idx 1))
        (cons (car lst)
              (slice-helper start end (cdr lst) (+ idx 1)))))))

; return sublist lst[start:end] (not inclusive)
(defun slice (start end lst)
  (slice-helper start end lst 0))

; sort a list of numbers
(defun merge-sort (lst)
  (if (null? lst)
    lst
    (if (null? (cdr lst))
      lst
      (let ((len (length lst))
            (half-len (/ len 2))
            (left (slice 0 half-len lst))
            (right (slice half-len len lst)))
        (merge (merge-sort left) (merge-sort right))))))

(println (merge-sort (list 42 7 89 56 13 78 31 64 5 90)))
