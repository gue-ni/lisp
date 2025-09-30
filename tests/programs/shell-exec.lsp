;
(println (exec "expr" 2 "+" 3))


(println (exec "ls" "-la"))


; implement pipe as a macro to set the correct input and output file descriptors to allow piping
(defmacro pipe (prog1 prog2)
  `(progn
    (,prog1)
    (,prog2)))


(pipe (exec "ls") (exec "whoami"))

