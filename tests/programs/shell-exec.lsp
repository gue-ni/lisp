
(println (exec "expr" 2 "+" 3))

(println (exec  "uptime"))

(println "pipe 1:")

(defun my-grep (n)
  (exec "grep" n))

(println (pipe (exec "find" ".") (exec "grep" ".cpp")))

(println "pipe 2:")

; does not work -> i think there is a problem of how clojures are handled
; right now, when the function is called it only has access to the variables from the closure
; (println (pipe (exec "find" ".") (my-grep ".cpp")))

