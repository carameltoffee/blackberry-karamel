#include "lexer.hpp"
#include <cctype>

const std::unordered_set<std::string> Lexer::control_keywords = {
    "if", "else", "for", "return"};

const std::unordered_set<std::string> Lexer::types = {
    "num", "flo", "str", "bool"};

const std::string Lexer::function_keyword = "fn";

Lexer::Lexer(std::string source) : source(std::move(source)) {}

void Lexer::reset()
{
     pos = 0;
     line = 1;
     column = 1;
}

char Lexer::peek() const
{
     if (pos >= source.size())
          return '\0';
     return source[pos];
}

char Lexer::get()
{
     if (pos >= source.size())
          return '\0';
     char c = source[pos++];
     if (c == '\n')
     {
          line++;
          column = 1;
     }
     else
     {
          column++;
     }
     return c;
}

void Lexer::skip_whitespace()
{
     while (std::isspace(peek()))
          get();
}

bool Lexer::is_keyword(const std::string &str) const
{
     return control_keywords.find(str) != control_keywords.end();
}

Token Lexer::next_token()
{
     skip_whitespace();

     size_t token_line = line;
     size_t token_col = column;
     char c = peek();

     if (c == '\0')
     {
          return {TokenType::EndOfFile, "", token_line, token_col};
     }

     if (c == '#')
     {
          while (peek() != '\0' && peek() != '\n')
          {
               get();
          }
          return next_token();
     }

     if (std::isalpha(c) || c == '_')
     {
          std::string ident;
          while (std::isalnum(peek()) || peek() == '_')
          {
               ident += get();
          }

          if (ident == function_keyword)
          {
               return {TokenType::Function, ident, token_line, token_col};
          }
          if (types.find(ident) != types.end())
          {
               return {TokenType::Type, ident, token_line, token_col};
          }
          if (control_keywords.find(ident) != control_keywords.end())
          {
               if (ident == "if")
                    return {TokenType::If, ident, token_line, token_col};
               if (ident == "else")
                    return {TokenType::Else, ident, token_line, token_col};
               if (ident == "for")
                    return {TokenType::For, ident, token_line, token_col};
               if (ident == "return")
                    return {TokenType::Return, ident, token_line, token_col};
          }

          if (ident == "true" || ident == "false")
          {
               return {TokenType::Boolean, ident, token_line, token_col};
          }

          return {TokenType::Identifier, ident, token_line, token_col};
     }

     if (std::isdigit(c) || (c == '-' && std::isdigit(source[pos + 1])))
     {
          std::string number;
          bool has_dot = false;

          if (peek() == '-')
               number += get();

          while (std::isdigit(peek()) || (peek() == '.' && !has_dot))
          {
               if (peek() == '.')
               {
                    has_dot = true;
               }
               number += get();
          }

          return {has_dot ? TokenType::Float : TokenType::Integer, number, token_line, token_col};
     }

     if (c == '"' || c == '\'')
     {
          char quote = get();
          std::string str;
          while (peek() != quote && peek() != '\0' && peek() != '\n')
          {
               str += get();
          }
          if (peek() == quote)
          {
               get();
          }
          return {TokenType::String, str, token_line, token_col};
     }

     if (std::string("+-*/=<>!").find(c) != std::string::npos)
     {
          std::string op;
          op += get();
          if (std::string("=<>!").find(op[0]) != std::string::npos && peek() == '=')
          {
               op += get();
          }
          return {TokenType::Operator, op, token_line, token_col};
     }

     if (c == ':')
     {
          char colon = get();
          return {TokenType::OfType, std::string(1, colon), token_line, token_col};
     }

     if (std::string("(){}[],;").find(c) != std::string::npos)
     {
          std::string punct;
          punct += get();
          return {TokenType::Punctuation, punct, token_line, token_col};
     }

     std::string unknown;
     unknown += get();
     return {TokenType::Unknown, unknown, token_line, token_col};
}