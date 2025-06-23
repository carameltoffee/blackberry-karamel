#include "parser.hpp"
#include <stdexcept>
#include <iostream>

Parser::Parser(Lexer lexer) : lexer(std::move(lexer))
{
     advance();
}

void Parser::advance()
{
     current = lexer.next_token();
}

bool Parser::match(TokenType type, const std::string &val)
{
     return current.type == type && (val.empty() || current.value == val);
}

void Parser::expect(TokenType type, const std::string &val)
{
     if (!match(type, val))
     {
          throw std::runtime_error("Unexpected token: " + current.to_string() + " Waited for: " + val);
     }
     advance();
}
std::string node_type_to_string(NodeType type)
{
     switch (type)
     {
     case NodeType::FunctionDecl:
          return "Function";
     case NodeType::FunctionCall:
          return "Call";
     case NodeType::If:
          return "If";
     case NodeType::For:
          return "For";
     case NodeType::Return:
          return "Return";
     case NodeType::Assignment:
          return "Assignment";
     case NodeType::Identifier:
          return "Identifier";
     case NodeType::Number:
          return "Number";
     case NodeType::String:
          return "String";
     case NodeType::Boolean:
          return "Boolean";
     case NodeType::BinaryOp:
          return "BinaryOp";
     case NodeType::ParamList:
          return "ParamList";
     default:
          return "???";
     }
}

void print_ast(const std::shared_ptr<ASTNode> &node, int indent = 0)
{
     std::string ind(indent * 2, ' ');
     std::cout << ind << "Node: " << node->value << " type: (" + node_type_to_string(node->type) + ")\n";
     for (const auto &child : node->children)
     {
          print_ast(child, indent + 1);
     }
}


std::vector<std::shared_ptr<ASTNode>> Parser::parse()
{
     std::vector<std::shared_ptr<ASTNode>> nodes;
     while (!match(TokenType::EndOfFile))
     {
          auto node = parse_statement();
          // print_ast(node);
          nodes.push_back(node);
     }
     return nodes;
}
std::shared_ptr<ASTNode> Parser::parse_statement()
{
     if (match(TokenType::Function))
     {
          return parse_function_decl();
     }

     if (match(TokenType::Return))
     {
          advance();
          auto node = std::make_shared<ASTNode>(NodeType::Return, "return");
          node->children.push_back(parse_expression());
          expect(TokenType::Punctuation, ";");
          return node;
     }

     if (match(TokenType::If))
     {
          auto node = std::make_shared<ASTNode>(NodeType::If, current.value);
          advance();
          expect(TokenType::Punctuation, "(");
          node->children.push_back(parse_expression());
          expect(TokenType::Punctuation, ")");
          node->children.push_back(parse_block());
          return node;
     }

     if (match(TokenType::For))
     {
          advance();
          expect(TokenType::Punctuation, "(");

          auto for_node = std::make_shared<ASTNode>(NodeType::For, "for");

          auto first_expr = parse_expression();

          if (match(TokenType::Punctuation, ";"))
          {
               advance();
               auto second_expr = parse_expression();

               auto loop = std::make_shared<ASTNode>(NodeType::ForLoop, "loop");
               loop->children.push_back(first_expr);
               loop->children.push_back(second_expr);

               for_node->children.push_back(loop);
          }
          else
          {
               auto cond = std::make_shared<ASTNode>(NodeType::While, "while");
               cond->children.push_back(first_expr);
               for_node->children.push_back(cond);
          }

          expect(TokenType::Punctuation, ")");
          for_node->children.push_back(parse_block());

          return for_node;
     }

     auto expr = parse_expression();
     expect(TokenType::Punctuation, ";");
     return expr;
}

std::shared_ptr<ASTNode> Parser::parse_function_decl()
{
     expect(TokenType::Function);
     std::string name = current.value;
     expect(TokenType::Identifier);

     auto func = std::make_shared<ASTNode>(NodeType::FunctionDecl, name);

     expect(TokenType::Punctuation, "(");

     auto param_list = std::make_shared<ASTNode>(NodeType::ParamList, "");
     if (!match(TokenType::Punctuation, ")"))
     {
          while (true)
          {
               std::string param_name = current.value;
               expect(TokenType::Identifier);
               expect(TokenType::OfType);
               std::string type = current.value;
               expect(TokenType::Type);

               param_list->children.push_back(
                   std::make_shared<ASTNode>(NodeType::Identifier, param_name));

               if (match(TokenType::Punctuation, ","))
               {
                    advance();
                    if (match(TokenType::Punctuation, ")"))
                    {
                         throw std::runtime_error("Unexpected trailing comma in function parameters at " + current.to_string());
                    }
               }
               else
               {
                    break;
               }
          }
     }

     expect(TokenType::Punctuation, ")");
     func->children.push_back(param_list);

     if (match(TokenType::Type))
     {
          func->children.push_back(
              std::make_shared<ASTNode>(NodeType::Identifier, "ret:" + current.value));
          advance();
     }

     func->children.push_back(parse_block());

     return func;
}

std::shared_ptr<ASTNode> Parser::parse_block()
{
     expect(TokenType::Punctuation, "(");
     std::vector<std::shared_ptr<ASTNode>> stmts;
     while (!match(TokenType::Punctuation, ")"))
     {
          stmts.push_back(parse_statement());
     }
     expect(TokenType::Punctuation, ")");
     auto block = std::make_shared<ASTNode>(NodeType::Identifier, "block");
     block->children = std::move(stmts);
     return block;
}

std::shared_ptr<ASTNode> Parser::parse_expression()
{
     auto left = parse_primary();

     if (left->type == NodeType::Identifier && match(TokenType::Operator, "="))
     {
          advance();
          auto assign = std::make_shared<ASTNode>(NodeType::Assignment, "=");
          assign->children.push_back(left);
          assign->children.push_back(parse_expression());
          return assign;
     }

     while (true)
     {
          if (match(TokenType::Operator))
          {
               std::string op = current.value;
               advance();
               auto right = parse_primary();
               auto bin = std::make_shared<ASTNode>(NodeType::BinaryOp, op);
               bin->children.push_back(left);
               bin->children.push_back(right);
               left = bin;
          }
          else if (match(TokenType::Punctuation, ","))
          {
               // advance();
               break;
          }
          else if (match(TokenType::Punctuation, ")"))
          {
               break;
          }
          else
          {
               break;
          }
     }

     return left;
}

std::shared_ptr<ASTNode> Parser::parse_primary()
{
     if (match(TokenType::Identifier))
     {
          std::string name = current.value;
          advance();

          if (match(TokenType::Punctuation, "("))
          {
               advance();

               auto call = std::make_shared<ASTNode>(NodeType::FunctionCall, name);

               while (!match(TokenType::Punctuation, ")") && !match(TokenType::EndOfFile))
               {
                    call->children.push_back(parse_expression());

                    if (match(TokenType::Punctuation, ","))
                    {
                         advance();
                    }
                    else if (!match(TokenType::Punctuation, ")"))
                    {
                         throw std::runtime_error("Expected ',' or ')' in function arguments, got: " + current.to_string());
                    }
               }

               expect(TokenType::Punctuation, ")");
               return call;
          }

          return std::make_shared<ASTNode>(NodeType::Identifier, name);
     }

     if (match(TokenType::Punctuation, "("))
     {
          advance();
          auto expr = parse_expression();
          expect(TokenType::Punctuation, ")");
          return expr;
     }

     if (match(TokenType::Integer) || match(TokenType::Float))
     {
          auto node = std::make_shared<ASTNode>(NodeType::Number, current.value);
          advance();
          return node;
     }

     if (match(TokenType::String))
     {
          auto node = std::make_shared<ASTNode>(NodeType::String, current.value);
          advance();
          return node;
     }

     if (match(TokenType::Boolean))
     {
          auto node = std::make_shared<ASTNode>(NodeType::Boolean, current.value);
          advance();
          return node;
     }

     throw std::runtime_error("Unexpected token in expression: " + current.to_string());
}
