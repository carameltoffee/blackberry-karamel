#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include "parser.hpp"

class Runner
{
public:
     void interpret(const std::vector<std::shared_ptr<ASTNode>> &nodes);

     struct Value
     {
          enum class Type
          {
               Int,
               String,
               Bool,
               None
          } type;

          int int_val;
          std::string str_val;
          bool bool_val;

          Value();

          explicit Value(int v);
          explicit Value(std::string v);
          explicit Value(bool v);

          std::string to_string() const;
          bool is_number() const
          {
               return type == Type::Int;
          }
     };
     struct ReturnSignal
     {
          Value value;
     };

private:
     std::vector<std::unordered_map<std::string, Value>> scopes;
     std::unordered_map<std::string, std::shared_ptr<ASTNode>> functions;

     Value execute(const std::shared_ptr<ASTNode> &node);
     Value execute_for(const std::shared_ptr<ASTNode> &node);
     Value eval_binary_op(const std::string &op, const Value &left, const Value &right);
     bool is_true(const Value &val);
     Value execute_block(const std::shared_ptr<ASTNode> &block);
     Value execute_function_call(const std::shared_ptr<ASTNode> &node);
     void set_variable(const std::string &name, const Value &val);
     Value get_variable(const std::string &name);
};
