(defmacro symbolp (s)
  `(let ((e ,s)) (not (error? e))))

(defmacro sh-arg (x)
  `(if (symbolp ,x) ,x (symbol-name x)))

(defmacro shell (&args)
  `(apply exec (map sh-arg args)))

