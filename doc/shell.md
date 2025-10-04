# Shell Scripting

_Lisp_ includes common linux shell scripting implements, including piping, redirecting
and capturing output of programs.

## Getting Started

## Utilities

### `exec`

```lisp
```

### `sh`

```lisp
; execute the 'uname -a' command
(sh uname -a)
```


### `pipe`

```lisp
(pipe
  (sh ls -la )
  (sh grep "*.txt"))
```

### `to-stream`

```lisp
```

### `from-stream`

```lisp
```

