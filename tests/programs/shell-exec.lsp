

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

(define ls-output (from-stream (exec "ls")))

(println "capture 1:")
(println ls-output)


(println "capture 2:")
(println (pipe (exec "find" "src") (from-stream (my-grep ".h"))))

(println "capture 3:")
(define output
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
(define my-str (from-stream (pipe (to-stream "hello world") (exec "rev"))))
(println "test: " my-str)

(println "to-pipe 3")
(define my-rev-str (from-stream (pipe (to-stream "hello world") (exec "rev"))))
(println "test: " my-rev-str)

; checks if a symbol is defined
(defmacro symbolp (s)
  `(let ((e ,s)) (not (error? e))))

;
(defmacro sh-arg (x)
  `(cond
    ((symbolp ,x) (symbol-name ,x))
    (true x)))



(println "should be false: " (symbolp foo))

(define bar 123)
(println "should be true: " (symbolp bar))

;(defmacro sh (& args)
;  `(exec ,@(map 'sh-arg args)))

(defmacro shell (&args)
  `(exec (map sh-arg args)))

(shell whoami)

;(println "shell: " (from-stream (shell uname -a)))


;(println "test: " (sh-arg 'uname))