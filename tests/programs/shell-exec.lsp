
(println (exec "expr" 2 "+" 3))

(println (exec  "uptime"))

(println "test command builder")

(defun my-grep (n)
  (exec "grep" n))

(defun command-builder (cmd)
  (lambda (arg) (exec cmd arg)))

(define ls (command-builder "ls"))

(ls "-la")

(println "pipe 1:")
(println (pipe (exec "find" "src") (exec "grep" ".cpp")))

; exec is called in a sub-program
(println "pipe 2:")
(println (pipe (exec "find" "src") (my-grep ".h")))

