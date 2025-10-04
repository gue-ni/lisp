# Shell Scripting

_Lisp_ includes common linux shell scripting implements, including piping, redirecting
and capturing output of programs.

## Getting Started

## Utilities

### `exec`

```lisp
```

### `sh`

Execute unix commands.

```lisp
; execute the 'uname -a' command
(sh uname -a)

; ls -la
(sh ls -la)
```

### `pipe`

Pipe output from one command into the input of another.

```lisp
(pipe
  (sh ls -la )
  (sh grep "*.txt"))
```

### `<<<`

Send the string version of an expression to _stdout_.

```lisp
; pipe 'hello world' into the 'rev' command
(pipe
  (<<< "hello world")
  (sh rev))
```

### `$`

Capture _stdin_ as string.

```lisp
; capture the output of the 'cat' command
(defvar content ($ (sh cat "my-file.txt")))
```

