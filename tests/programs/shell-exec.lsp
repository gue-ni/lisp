
(println (exec "expr" 2 "+" 3))

(println (exec  "uptime"))

(println "pipe 1:")

(defun my-grep (n)
  (exec "grep" n))

(println (pipe (exec "find" ".") (exec "grep" ".cpp")))

; exec is called in a sub-program
(println "pipe 2:")
(println (pipe (exec "find" ".") (my-grep ".cpp")))

