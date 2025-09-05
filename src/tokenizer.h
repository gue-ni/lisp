#pragma once

#include <string>
#include <vector>

namespace lisp {

enum TokenType {};

struct Token {
  TokenType type;
  std::string lexeme;
};

using Tokens = std::vector<Token>;

class Tokenizer {
public:
private:
};

} // namespace lisp