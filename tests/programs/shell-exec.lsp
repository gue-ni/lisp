;
;(println (exec "expr" 2 "+" 3))


;(println (exec  "uptime"))


(println "test my pipe:")


; implement pipe as a macro that splices in the file descriptors
; TODO how can i get the file descriptor into
(defmacro my-pipe (exec1 exec2)
  `(let ((fd (make-pipe)))
    (progn
      (println "pipe: " fd)
      (,(car exec1) ,@(cdr exec1) 10)
      (,(car exec2) ,@(cdr exec2) 20))))


(pipe
  (exec  "find" ".")
  (exec "grep" ".cpp"))

