#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <stdexcept>
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
               Float,
               None
          } type;

          int int_val;
          double float_val;
          std::string str_val;
          bool bool_val;

          Value();

          explicit Value(int v);
          explicit Value(std::string v);
          explicit Value(bool v);
          explicit Value(double v);

          std::string to_string() const
          {
               switch (type)
               {
               case Type::Int:
                    return std::to_string(int_val);
               case Type::String:
                    return str_val;
               case Type::Bool:
                    return bool_val ? "true" : "false";
               case Type::Float:
                    return std::to_string(float_val);
               default:
                    return "none";
               }
          }
          bool is_number() const
          {
               return (type == Type::Int) || (type == Type::Float);
          }
          void check_type(Type expected) const
          {
               if (type != expected)
               {
                    throw std::runtime_error("Type mismatch");
               }
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

     bool has_variable_in_current_scope(const std::string &name);
     void define_variable(const std::string &name, const Value &val);
     void set_variable(const std::string &name, const Value &val);
     Value get_variable(const std::string &name);

     bool matches_type(const Value &val, const std::string &expected_type);

     Value eval_num_op(const Value &left, const std::string &op, const Value &right);
     Value eval_string_op(const Value &left, const std::string &op, const Value &right);
     Value eval_bool_op(const Value &left, const std::string &op, const Value &right);
};
