; checks if a symbol is defined
;(defmacro symbolp (s)
;  `(let ((e ,s)) (not (error? e))))
;
;;
;(defmacro sh-arg (x)
;  `(if (symbolp ,x) ,x (symbol-name x)))
;
;
;(defmacro shell (&args)
;  `(apply exec (map sh-arg args)))

;(println "sh-arg: " (sh-arg 'ls))
;(println "sh-arg: " (string? (sh-arg 'ls)))
;(println )
;(println "shell")
;(shell ls -la)

(load "stdlib/shell-lib.lsp")


(define bar 123)
(println "should be false: " (symbolp foo))
(println "should be true: " (symbolp bar))


;(define readme "README.md")
; (println "readme: " readme)
; (println " : " (symbolp readme))
; (println " : " (sh-arg readme))
; (println "cat: " cat)
; (println " : " (symbolp cat))
; (println " : " (sh-arg cat))

;(shell cat readme)

(define md "README.md")
(shell cat md)



; for some reason this does NOT work
(println
  (let
    ((md-local "README.md"))
    (shell cat md-local)))
