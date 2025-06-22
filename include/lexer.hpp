#pragma once

#include <string>
#include <unordered_set>
#include "token.hpp"

class Lexer
{
public:
     static const std::unordered_set<std::string> control_keywords;
     static const std::unordered_set<std::string> types;
     static const std::string function_keyword;

     explicit Lexer(std::string source);

     Token next_token();
     void reset();

private:
     std::string source;
     size_t pos = 0;
     size_t line = 1;
     size_t column = 1;

     char peek() const;
     char get();
     void skip_whitespace();
     bool is_keyword(const std::string &str) const;
};
