(define pi 3.14159265359)

(define circle-area
  (lambda (r)
    (* pi (* r r))))

(println (circle-area 5))
