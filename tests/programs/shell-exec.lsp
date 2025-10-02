(define cap capture)
(define sh exec)

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

(define ls-output (capture (exec "ls")))

(println "capture 1:")
(println ls-output)


(println "capture 2:")
(println (pipe (exec "find" "src") (capture (my-grep ".h"))))

(println "capture 3:")
(println (capture (pipe (exec "find" "src") (my-grep ".h"))))


(println "to-pipe 1")
(println (pipe (exec "echo" "hello world") (exec "rev")))

(println "to-pipe 2")
(println (capture (pipe (to-stream "hello world") (exec "rev"))))

(println "to-pipe 3")
(define my-rev-str (capture (pipe (to-stream "hello world") (sh "rev"))))
(println my-rev-str)