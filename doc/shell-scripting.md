# Shell Scripting

## Content

- [Shell Scripting](#shell-scripting)
  - [Content](#content)
  - [Getting Started](#getting-started)
  - [Utilities](#utilities)
    - [`sh` - Execute shell commands](#sh---execute-shell-commands)
    - [`pipe` - Connect commands](#pipe---connect-commands)
    - [`$` - Capture _stdout_](#---capture-stdout)
    - [`<<<` - Feed expression to _stdout_](#---feed-expression-to-stdout)
    - [`exec` - Execute a list of commands](#exec---execute-a-list-of-commands)
    - [`and`/`or`](#andor)

_Lisp_ includes common linux shell scripting implements, including piping, redirecting
and capturing output of programs.

## Getting Started

## Utilities

### `sh` - Execute shell commands

Execute unix commands.

```lisp
; execute the 'uname -a' command
(sh uname -a)

; ls -la
(sh ls -la)
```

### `pipe` - Connect commands

Pipe output from one command into the input of another.

```lisp
; pipe output of 'ls' to 'grep'
(pipe (sh ls -la ) (sh grep "*.txt"))
```

### `$` - Capture _stdout_

Capture _stdin_ as string. Very similar to `VAR=$(<some command>)` in bash.

```lisp
; capture the output of the 'cat' command and safe to 'content' variable
(defvar content ($ (sh cat "my-file.txt")))
```

### `<<<` - Feed expression to _stdout_

Send the string version of an expression to _stdout_.

```lisp
; pipe 'hello world' into the 'rev' command
(pipe (<<< "hello world") (sh rev))
```

### `exec` - Execute a list of commands

Execute a list of strings as a shell command. This function is a wrapper around
the `execvp` function in linux. It is used internally by `sh`.

### `and`/`or`

Shell commands can be combined using `and` and `or` to get conditional
execution.

```lisp
; sudo apt update && sudo apt upgrade
(and (sh sudo apt update) (sh sudo apt upgrade))
```


