#include "runner.hpp"
#include <stdexcept>
#include <iostream>
#include <cmath>

Runner::Value::Value()
    : type(Type::None), int_val(0), bool_val(false)
{
}

Runner::Value::Value(int v)
    : type(Type::Int), int_val(v), bool_val(false)
{
}

Runner::Value::Value(std::string v)
    : type(Type::String), str_val(std::move(v)), bool_val(false)
{
}

Runner::Value::Value(bool v)
    : type(Type::Bool), int_val(0), bool_val(v)
{
}

Runner::Value::Value(double v)
    : type(Type::Float), float_val(v), bool_val(false)
{
}

void Runner::interpret(const std::vector<std::shared_ptr<ASTNode>> &nodes)
{
     scopes.clear();
     scopes.emplace_back();

     for (auto &node : nodes)
     {
          execute(node);
     }
}

Runner::Value Runner::execute(const std::shared_ptr<ASTNode> &node)
{
     switch (node->type)
     {
     case NodeType::Number:
          return Value(std::stoi(node->value));
     case NodeType::DecimalNumber:
          return Value(std::stod(node->value));
     case NodeType::String:
          return Value(node->value);
     case NodeType::Boolean:
          return Value(node->value == "true");
     case NodeType::Identifier:
     {
          auto val = get_variable(node->value);
          return val;
     }
     case NodeType::Assignment:
     {
          auto var_name = node->children[0]->value;
          auto val = execute(node->children[1]);
          if (has_variable_in_current_scope(var_name))
          {
               auto found_value = get_variable(var_name);
               found_value.check_type(val.type);
               set_variable(var_name, val);
          }
          else
          {
               define_variable(var_name, val);
          }
          return val;
     }
     case NodeType::BinaryOp:
     {
          auto left = execute(node->children[0]);
          auto right = execute(node->children[1]);
          return eval_binary_op(node->value, left, right);
     }
     case NodeType::For:
     {
          return execute_for(node);
          return Value();
     }
     case NodeType::FunctionCall:
     {
          return execute_function_call(node);
     }
     case NodeType::Return:
          throw ReturnSignal{execute(node->children[0])};
     case NodeType::If:
     {
          auto cond = execute(node->children[0]);
          if (is_true(cond))
          {
               return execute_block(node->children[1]);
          }
          return Value();
     }
     case NodeType::FunctionDecl:
     {
          const auto &name = node->value;
          functions[name] = node;
          return Value();
     }
     default:
          throw std::runtime_error("Unknown AST node type: " + to_string(node->type));
     }
}

Runner::Value Runner::execute_for(const std::shared_ptr<ASTNode> &node)
{
     if (node->children.empty())
          return Value();

     auto &first = node->children[0];
     auto &body = node->children[1];

     if (first->type == NodeType::While)
     {
          while (is_true(execute(first->children[0])))
          {
               execute_block(body);
          }
          return Value();
     }
     else if (first->type == NodeType::ForLoop)
     {
          if (first->children.size() != 2)
               throw std::runtime_error("Malformed for loop");

          execute(first->children[0]);

          auto assign = first->children[0];
          if (assign->type != NodeType::Assignment || assign->children.size() != 2)
               throw std::runtime_error("Expected assignment in for-loop initializer");

          auto var_node = assign->children[0];
          if (var_node->type != NodeType::Identifier)
               throw std::runtime_error("Expected identifier as loop variable");

          std::string var_name = var_node->value;

          while (true)
          {
               Value current = get_variable(var_name);
               Value limit = execute(first->children[1]);

               if (!current.is_number() || !limit.is_number())
               {
                    throw std::runtime_error("Loop bounds must be numeric types. Got: current = " + current.to_string() + ", limit = " + limit.to_string());
               }

               if (!(current.int_val < limit.int_val))
                    break;

               execute_block(body);

               set_variable(var_name, Value(current.int_val + 1));
          }

          return Value();
     }

     throw std::runtime_error("Invalid for-loop structure");
}

bool Runner::has_variable_in_current_scope(const std::string &name)
{
     if (scopes.empty())
          return false;
     return scopes.back().count(name) > 0;
}

Runner::Value Runner::eval_binary_op(const std::string &op, const Value &left, const Value &right)
{
     if (left.is_number() && right.is_number())
     {
          return eval_num_op(left, op, right);
     }
     if (left.type == Value::Type::Bool && right.type == Value::Type::Bool)
     {
          return eval_bool_op(left, op, right);
     }
     if (left.type == Value::Type::String && right.type == Value::Type::String)
     {
          return eval_string_op(left, op, right);
     }
     throw std::runtime_error("Unsupported operand types for operator: " + op);
}

Runner::Value Runner::eval_bool_op(const Value &left, const std::string &op, const Value &right)
{
     if (op == "&&")
     {
          return Value(left.bool_val && right.bool_val);
     }
     if (op == "||")
     {
          return Value(left.bool_val || right.bool_val);
     }
     throw std::runtime_error("Unsupported operator: " + op);
}

Runner::Value Runner::eval_string_op(const Value &left, const std::string &op, const Value &right)
{
     if (op == "+")
     {
          return Value(left.str_val + right.str_val);
     }
     throw std::runtime_error("Unsupported operator: " + op);
}

Runner::Value Runner::eval_num_op(const Value &left, const std::string &op, const Value &right)
{
     if (!left.is_number() || !right.is_number())
          throw std::runtime_error("Operands must be numbers");

     double lhs = (left.type == Value::Type::Float) ? left.float_val : left.int_val;
     double rhs = (right.type == Value::Type::Float) ? right.float_val : right.int_val;

     auto make_result = [](double result) -> Value
     {
          return (std::floor(result) == result) ? Value(static_cast<int>(result)) : Value(result);
     };

     if (op == "+")
          return make_result(lhs + rhs);
     if (op == "-")
          return make_result(lhs - rhs);
     if (op == "*")
          return make_result(lhs * rhs);
     if (op == "/")
          return Value(lhs / rhs);
     if (op == "==")
          return Value(lhs == rhs);
     if (op == "!=")
          return Value(lhs != rhs);
     if (op == "<")
          return Value(lhs < rhs);
     if (op == "<=")
          return Value(lhs <= rhs);
     if (op == ">")
          return Value(lhs > rhs);
     if (op == ">=")
          return Value(lhs >= rhs);

     throw std::runtime_error("Unsupported operator: " + op);
}

bool Runner::is_true(const Value &val)
{
     if (val.type == Value::Type::Bool)
          return val.bool_val;
     if (val.type == Value::Type::Int)
          return val.int_val != 0;
     if (val.type == Value::Type::String)
          return !val.str_val.empty();
     return false;
}

Runner::Value Runner::execute_block(const std::shared_ptr<ASTNode> &block)
{
     Value last;
     for (auto &stmt : block->children)
     {
          last = execute(stmt);
     }
     return last;
}

void Runner::define_variable(const std::string &name, const Value &val)
{
     if (scopes.empty())
          throw std::runtime_error("No active scope to define variable: " + name);

     auto &current = scopes.back();
     if (current.find(name) != current.end())
          throw std::runtime_error("Variable already defined in current scope: " + name);

     current[name] = val;
}

Runner::Value Runner::get_variable(const std::string &name)
{
     for (auto it = scopes.rbegin(); it != scopes.rend(); ++it)
     {
          auto found = it->find(name);
          if (found != it->end())
               return found->second;
     }
     throw std::runtime_error("Undefined variable: " + name);
}

void Runner::set_variable(const std::string &name, const Value &val)
{
     if (!scopes.empty())
     {
          scopes.back()[name] = val;
          return;
     }
     throw std::runtime_error("No active scope to set variable: " + name);
}

bool Runner::matches_type(const Value &val, const std::string &expected_type)
{
     if (expected_type == "num")
          return val.type == Value::Type::Int;
     if (expected_type == "flo")
          return val.type == Value::Type::Float;
     if (expected_type == "str")
          return val.type == Value::Type::String;
     if (expected_type == "bool")
          return val.type == Value::Type::Bool;
     return false;
}

std::pair<std::string, std::string> extract_name_and_type(const std::string &s)
{
     auto pos = s.find(':');
     if (pos == std::string::npos || pos + 1 >= s.size())
          throw std::runtime_error("Malformed type annotation: " + s);

     std::string name = s.substr(0, pos);
     std::string type = s.substr(pos + 1);
     return {name, type};
}

Runner::Value Runner::execute_function_call(const std::shared_ptr<ASTNode> &node)
{
     if (node->value == "cout")
     {
          for (auto &arg : node->children)
          {
               auto val = execute(arg);
               std::cout << val.to_string();
          }
          std::cout << std::endl;
          return Value();
     }

     auto it = functions.find(node->value);
     if (it == functions.end())
          throw std::runtime_error("Unknown function: " + node->value);

     const auto &func_decl = it->second;
     const auto &param_list = func_decl->children[0];
     const auto &body = func_decl->children.back();

     if (param_list->children.size() != node->children.size())
     {
          std::cerr << "Argument count mismatch\n";
          std::exit(1);
     }

     std::vector<Value> arg_values;
     for (auto &arg_node : node->children)
     {
          arg_values.push_back(execute(arg_node));
     }

     scopes.emplace_back();

     for (size_t i = 0; i < param_list->children.size(); ++i)
     {
          auto [name, type] = extract_name_and_type(param_list->children[i]->value);
          const Value &arg = arg_values[i];

          if (!matches_type(arg, type))
          {
               throw std::runtime_error("Type mismatch for argument");
          }

          define_variable(name, arg);
     }

     Value result;

     std::string return_type;
     if (func_decl->children.size() > 2)
     {
          const std::string &raw = func_decl->children[1]->value;
          auto [name, type] = extract_name_and_type(raw);

          return_type = type;
     }

     try
     {
          result = execute_block(body);
     }
     catch (const ReturnSignal &r)
     {
          result = r.value;
     }

     if (!return_type.empty())
     {
          if (!matches_type(result, return_type))
          {
               throw std::runtime_error("Return type mismatch");
          }
     }

     scopes.pop_back();
     return result;
}
