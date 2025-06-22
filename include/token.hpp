#pragma once

#include <string>
#include <string_view>

enum class TokenType
{
     Integer,
     Float,
     String,
     Boolean,

     Identifier,
     Function, 
     Type,    

     If,
     OfType,
     Else,
     For,
     Return,

     Operator,
     Punctuation,

     EndOfFile,
     Unknown
};

struct Token
{
     TokenType type;
     std::string value;
     size_t line;
     size_t column;

     std::string to_string() const;
};
