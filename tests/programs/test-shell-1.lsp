

(println (exec "expr" 2 "+" 3))

(println (exec  "uptime"))

(println "test command builder")

(defun my-grep (n)
  (exec "grep" n))

(defun command-builder (cmd)
  (lambda (arg) (exec cmd arg)))

(defvar ls (command-builder "ls"))

(ls "-la")

(println "pipe 1:")
(println (pipe (exec "find" "src") (exec "grep" ".cpp")))

; exec is called in a sub-program
(println "pipe 2:")
(println (pipe (exec "find" "src") (my-grep ".h")))

(defvar ls-output (from-stream (exec "ls")))

(println "capture 1:")
(println ls-output)


(println "capture 2:")
(println (pipe (exec "find" "src") (from-stream (my-grep ".h"))))

(println "capture 3:")
(defvar output
  (from-stream
    (pipe
      (pipe
        (exec "find" "src")
        (my-grep ".h"))
      (exec "grep" "eval"))))

(println "output: " output)


(println "to-pipe 1")
(println (pipe (exec "echo" "hello world") (exec "rev")))

(println "to-pipe 2")
(defvar my-str (from-stream (pipe (to-stream "hello world") (exec "rev"))))
(println "test: " my-str)

(println "to-pipe 3")
(defvar my-rev-str (from-stream (pipe (to-stream "hello world") (exec "rev"))))
(println "test: " my-rev-str)
