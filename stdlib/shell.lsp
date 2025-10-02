; test if a symbol is defined
(defmacro symbolp (s)
  `(let ((e ,s)) (not (error? e))))

; if symbol is defined, return the symbol value, otherwise return the symbol name as a string
(defmacro sh-arg (x)
  `(if (symbolp ,x) ,x (symbol-name x)))

; execute shell command, no quotes needed
; usage:
; (sh ls -la)
; (let ((my-file "my_file.txt")) (sh touch my-file))
(defmacro sh (&args)
  `(apply exec (map sh-arg args)))

