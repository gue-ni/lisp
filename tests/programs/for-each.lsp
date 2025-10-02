
(define my-list (list 1 2 3 4 5))
;
(defmacro for-each-1 (lst)
  (if (null? lst)
   nil
   `(progn
      (defun my-loop (l)
         (if (null? l)
            nil
            (progn
               (let ((el (car l))) (println el))
               (my-loop (cdr l)))))
      (my-loop ,lst))))

(for-each-1 my-list)
