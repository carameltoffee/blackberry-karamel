#include "lexer.hpp"
#include "parser.hpp"
#include <iostream>
#include <runner.hpp>

std::string Token::to_string() const
{
     std::string type_str;
     switch (type)
     {
     case TokenType::Identifier:
          type_str = "Identifier";
          break;
     case TokenType::Function:
          type_str = "Function";
          break;
     case TokenType::Type:
          type_str = "Type";
          break;
     case TokenType::If:
          type_str = "If";
          break;
     case TokenType::Else:
          type_str = "Else";
          break;
     case TokenType::For:
          type_str = "For";
          break;
     case TokenType::Return:
          type_str = "Return";
          break;
     case TokenType::Boolean:
          type_str = "Boolean";
          break;
     case TokenType::Integer:
          type_str = "Integer";
          break;
     case TokenType::Float:
          type_str = "Float";
          break;
     case TokenType::String:
          type_str = "String";
          break;
     case TokenType::Operator:
          type_str = "Operator";
          break;
     case TokenType::Punctuation:
          type_str = "Punctuation";
          break;
     case TokenType::EndOfFile:
          type_str = "EOF";
          break;
     case TokenType::OfType:
          type_str = "OfType";
          break;
     case TokenType::Unknown:
          type_str = "Unknown";
          break;
     }

     return type_str + "('" + value + "') at " +
            std::to_string(line) + ":" + std::to_string(column);
}

// void print_ast(const std::shared_ptr<ASTNode> &node, int indent = 0)
// {
//      std::string pad(indent, ' ');
//      std::cout << pad << node_type_to_string(node->type);
//      if (!node->value.empty())
//           std::cout << ": " << node->value;
//      std::cout << "\n";

//      for (const auto &child : node->children)
//      {
//           print_ast(child, indent + 2);
//      }
// }

int main()
{
     std::string code = R"(fn mul_nums(x: flo, y: num) num (
    return (x+y)*2;
)

fn fact(n: num) (
    if (n <= 1) (
        return 1;
    )
    return n*fact(n-1);
)

x = 'be';
# x = 10;
cout(x);

cout(fact(11));
cout(mul_nums(1.0, 2));
# cout(10.0 / 0);

)";

     Lexer lexer(code);
     Parser parser(lexer);
     Runner runner;
     auto ast = parser.parse();
     runner.interpret(ast);

     return 0;
}
