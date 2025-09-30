#include "shell.h"
#include "expr.h"
#include <cstddef>
#include <string>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

namespace lisp {

namespace shell {

Expr *f_exec(Expr *arg, Context &context, const IO &io) {

  std::cout << arg->to_json() << std::endl;

  int status = -1;

  pid_t pid = fork();

  if (pid == 0) {

    std::vector<char *> exec_args;

    for (Expr *it = arg; it->is_cons(); it = it->cdr()) {
      Expr *el = it->car();
      Expr *str = el->is_string() ? el : cast_to_string(el);
      exec_args.push_back(const_cast<char *>(str->as_string()));
    }
    exec_args.push_back(nullptr);

    char *args[] = {const_cast<char *>("ls"), const_cast<char *>("-l"),
                    nullptr};

    errno = 0;
    execvp(exec_args[0], exec_args.data());

    if (errno != 0) {
      return make_error(strerror(errno));
    }

  } else if (pid > 0) {
    waitpid(pid, &status, 0);
  } else {
    perror("fork failed");
    return make_error("fork failed");
  }

  return make_list(make_number((double)status));
}

Expr *f_pipe(Expr *arg, Context &context, const IO &io) {
  std::cout << arg->to_json() << std::endl;
  return make_nil();
}

} // namespace shell

} // namespace lisp