(load "stdlib/shell.lsp")

(pipe
  (sh ls)
  (sh grep ".txt"))
