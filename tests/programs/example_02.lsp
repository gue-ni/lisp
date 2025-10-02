(defvar pi 3.14159265359)

(defvar circle-area
  (lambda (r)
    (* pi (* r r))))

(println (circle-area 5))
