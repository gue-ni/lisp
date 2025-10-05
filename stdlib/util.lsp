; get first element of a list
(defun first (lst)
  (car lst))

; get rest of a list
(defun rest (lst)
  (cdr lst))

; get second element of a list
(defun second (lst)
  (car (cdr lst)))

; get third element of a list
(defun third (lst)
  (car (cdr (cdr lst))))

; get last element of a list
(defun last (lst)
  (if (null? (cdr lst))
    (car lst)
    (last (cdr lst))))

; get nth element of list
(defun nth (n lst)
  (cond
    ((null? lst) nil)
    ((= n 0) (car lst))
    (t (nth (- n 1) (cdr lst)))))
