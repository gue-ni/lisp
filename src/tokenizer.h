#pragma once

#include <string>
#include <vector>

enum TokenType {};

struct Token {
  TokenType type;
  std::string lexeme;
};

using Tokens = std::vector<Token>;