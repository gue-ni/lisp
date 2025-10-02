; first element of a list
(defun first (lst)
  (car lst))

; second element of a list
(defun second (lst)
  (car (cdr lst)))

; third element of a list
(defun third (lst)
  (car (cdr (cdr lst))))

