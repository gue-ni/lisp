; checks if a symbol is defined
(defmacro symbolp (s)
  `(let ((e ,s)) (error? e)))

;
(defmacro sh-arg (x)
  `(cond
    ((symbolp x) (symbol-name x))
    (true x)))



(println "should be false: " (symbolp foo))

(define bar 123)
(println "should be true: " (symbolp bar))

;(defmacro sh (& args)
;  `(exec ,@(map 'sh-arg args)))

(defmacro shell (&args)
  `(apply exec (map sh-arg args)))

(println "sh-arg: " (sh-arg 'ls))
(println "sh-arg: " (string? (sh-arg 'ls)))

(println "shell")
(println "shell: " (shell ls -la))

;(println "shell: " (from-stream (shell uname -a)))


;(println "test: " (sh-arg 'uname))

(println (apply + (list 1 2 3)))