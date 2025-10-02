; first element of a list
(defun first (lst)
  (car lst))

; second element of a list
(defun second (lst)
  (car (cdr lst)))

; third element of a list
(defun third (lst)
  (car (cdr (cdr lst))))

(defun last (lst)
  (if (null? (cdr lst))
    (car lst)
    (last (cdr lst))))

; get nth element of list
(defun nth (n lst)
  (cond
    ((null? lst)
      nil)
    ((= n 0)
      (car lst))
    (true
      (nth (- n 1) (cdr lst)))))
