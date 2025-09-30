;
(println (exec "expr" 2 "+" 3))


(println (exec "ls" "-la"))


(println "test my pipe:")


; implement pipe as a macro to set the correct input and output file descriptors to allow piping
; TODO how can i get the file descriptor into
(defmacro pipe (exec1 exec2)
  `(let ((fd (make-pipe)))
    (progn
      (println "pipe: " fd)
      (,exec1)
      (,exec2))))


(pipe (exec "ls" "-la") (exec "whoami"))

