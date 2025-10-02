(load "stdlib/shell.lsp")

(println "show readme:")
(define readme "README.md")
(sh cat readme)

(println "show gitignore:")
(let
  ((gitignore ".gitignore"))
  (sh cat gitignore))

(let ((my-file "my_file.txt"))
  (sh touch my-file))