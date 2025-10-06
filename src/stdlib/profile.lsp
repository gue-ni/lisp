
(let ((home (getenv "HOME"))
      (lib (strcat home "/.local/share/lisp/lib/")))
  (progn
    (load (strcat lib "util.lsp"))
    (load (strcat lib "shell.lsp"))))
