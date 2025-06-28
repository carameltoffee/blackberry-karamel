#include "interpreter/evaluator.hpp"
#include "interpreter/function_manager.hpp"
#include <stdexcept>
#include <cmath>
#include <iostream>

Evaluator::Evaluator(FunctionManager &fn_manager)
    : function_manager(fn_manager)
{
     scope_mgr.push_scope();
}

void Evaluator::push_scope()
{
     scope_mgr.push_scope();
}

void Evaluator::pop_scope()
{
     scope_mgr.pop_scope();
}

void Evaluator::define_variable(const std::string &name, const Value &value)
{
     scope_mgr.define(name, value);
}

void Evaluator::set_variable(const std::string &name, const Value &value)
{
     scope_mgr.set(name, value);
}

Value Evaluator::get_variable(const std::string &name) const
{
     return scope_mgr.get(name);
}

bool Evaluator::is_true(const Value &val) const
{
     switch (val.type)
     {
     case Value::Type::Bool:
          return val.bool_val;
     case Value::Type::Int:
          return val.int_val != 0;
     case Value::Type::String:
          return !val.str_val.empty();
     default:
          return false;
     }
}

Value Evaluator::evaluate(const std::shared_ptr<ASTNode> &node)
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
     case NodeType::Array:
     {
          std::vector<Value> vals;
          for (const auto &child : node->children)
               vals.push_back(evaluate(child));
          return Value(vals);
     }
     case NodeType::Identifier:
          return scope_mgr.get(node->value);
     case NodeType::Assignment:
     {
          auto var_name = node->children[0]->value;
          auto val = evaluate(node->children[1]);

          if (scope_mgr.has_in_current(var_name))
          {
               auto found_value = scope_mgr.get(var_name);
               found_value.check_type(val.type);
               scope_mgr.set(var_name, val);
          }
          else
          {
               scope_mgr.define(var_name, val);
          }

          return val;
     }

     case NodeType::BinaryOp:
     {
          auto left = evaluate(node->children[0]);
          auto right = evaluate(node->children[1]);
          return eval_binary_op(node->value, left, right);
     }
     case NodeType::If:
     {
          auto condition = evaluate(node->children[0]);
          if (is_true(condition))
               return evaluate_block(node->children[1]);
          return Value();
     }
     case NodeType::For:
          return execute_for(node);
     case NodeType::While:
          return execute_while(node->children[0], node->children[1]);
     case NodeType::FunctionCall:
          return function_manager.call(node->value, node->children, *this);
     case NodeType::FunctionDecl:
          function_manager.register_function(node->value, node);
          return Value();
     case NodeType::Return:
          throw ReturnSignal{evaluate(node->children[0])};
     default:
          throw std::runtime_error("Unknown AST node type");
     }
}

Value Evaluator::evaluate_block(const std::shared_ptr<ASTNode> &block)
{
     Value last;
     for (const auto &stmt : block->children)
          last = evaluate(stmt);
     return last;
}

Value Evaluator::execute_for(const std::shared_ptr<ASTNode> &node)
{
     auto &first = node->children[0];
     auto &body = node->children[1];

     if (first->type == NodeType::While)
          return execute_while(first->children[0], body);

     if (first->type == NodeType::ForLoop)
     {
          if (first->children.size() != 2)
               throw std::runtime_error("Malformed for loop");

          evaluate(first->children[0]);

          auto assign = first->children[0];
          std::string var_name = assign->children[0]->value;

          return execute_for_loop(body, first->children[1], var_name);
     }

     throw std::runtime_error("Invalid for-loop structure");
}

Value Evaluator::execute_for_loop(const std::shared_ptr<ASTNode> &body,
                                  const std::shared_ptr<ASTNode> &limit,
                                  const std::string &var_name)
{
     while (true)
     {
          Value current = scope_mgr.get(var_name);
          Value max_val = evaluate(limit);

          if (!current.is_number() || !max_val.is_number())
               throw std::runtime_error("Loop bounds must be numeric");

          if (current.int_val >= max_val.int_val)
               break;

          evaluate_block(body);
          scope_mgr.set(var_name, Value(current.int_val + 1));
     }
     return Value();
}

Value Evaluator::execute_while(const std::shared_ptr<ASTNode> &cond,
                               const std::shared_ptr<ASTNode> &body)
{
     while (is_true(evaluate(cond)))
          evaluate_block(body);
     return Value();
}

Value Evaluator::eval_binary_op(const std::string &op, const Value &lhs, const Value &rhs)
{
     if (lhs.is_number() && rhs.is_number())
          return eval_number_op(lhs, op, rhs);
     if (lhs.type == Value::Type::Bool && rhs.type == Value::Type::Bool)
          return eval_bool_op(lhs, op, rhs);
     if (lhs.type == Value::Type::String && rhs.type == Value::Type::String)
          return eval_string_op(lhs, op, rhs);

     throw std::runtime_error("Unsupported binary op for operand types");
}

Value Evaluator::eval_number_op(const Value &lhs, const std::string &op, const Value &rhs)
{
     double l = lhs.type == Value::Type::Float ? lhs.float_val : lhs.int_val;
     double r = rhs.type == Value::Type::Float ? rhs.float_val : rhs.int_val;

     auto result = [&](double val) -> Value
     {
          return std::floor(val) == val ? Value((int)val) : Value(val);
     };

     if (op == "+")
          return result(l + r);
     if (op == "-")
          return result(l - r);
     if (op == "*")
          return result(l * r);
     if (op == "/")
          return Value(l / r);
     if (op == "==")
          return Value(l == r);
     if (op == "!=")
          return Value(l != r);
     if (op == "<")
          return Value(l < r);
     if (op == "<=")
          return Value(l <= r);
     if (op == ">")
          return Value(l > r);
     if (op == ">=")
          return Value(l >= r);

     throw std::runtime_error("Unsupported numeric operator: " + op);
}

Value Evaluator::eval_string_op(const Value &lhs, const std::string &op, const Value &rhs)
{
     if (op == "+")
          return Value(lhs.str_val + rhs.str_val);
     throw std::runtime_error("Unsupported string operator: " + op);
}

Value Evaluator::eval_bool_op(const Value &lhs, const std::string &op, const Value &rhs)
{
     if (op == "&&")
          return Value(lhs.bool_val && rhs.bool_val);
     if (op == "||")
          return Value(lhs.bool_val || rhs.bool_val);
     throw std::runtime_error("Unsupported boolean operator: " + op);
}
