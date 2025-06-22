#pragma once
#include "token.hpp"
#include "lexer.hpp"
#include <memory>
#include <vector>

enum class NodeType
{
     FunctionDecl,
     FunctionCall,
     Return,
     If,
     For,
     Identifier,
     Number,
     String,
     Boolean,
     BinaryOp,
     Assignment
};

struct ASTNode
{
     NodeType type;
     std::string value;
     std::vector<std::shared_ptr<ASTNode>> children;

     ASTNode(NodeType t, std::string v) : type(t), value(std::move(v)) {}
};

class Parser
{
public:
     explicit Parser(Lexer lexer);

     std::vector<std::shared_ptr<ASTNode>> parse();

private:
     Lexer lexer;
     Token current;

     void advance();
     bool match(TokenType type, const std::string &val = "");
     void expect(TokenType type, const std::string &val = "");

     std::shared_ptr<ASTNode> parse_function_decl();
     std::shared_ptr<ASTNode> parse_statement();
     std::shared_ptr<ASTNode> parse_expression();
     std::shared_ptr<ASTNode> parse_block();
     std::shared_ptr<ASTNode> parse_primary();
};
