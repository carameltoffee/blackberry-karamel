#pragma once
#include <memory>
#include <vector>
#include "value.hpp"
#include <parser.hpp>
#include "scope_manager.hpp"

class FunctionManager;

struct ReturnSignal
{
     Value value;
     explicit ReturnSignal(Value v) : value(std::move(v)) {}
};

class Evaluator
{
public:
     Evaluator(FunctionManager &func_mgr);

     Value evaluate(const std::shared_ptr<ASTNode> &node);
     Value evaluate_block(const std::shared_ptr<ASTNode> &block);

     void push_scope();
     void pop_scope();

     void define_variable(const std::string &name, const Value &value);
     void set_variable(const std::string &name, const Value &value);
     Value get_variable(const std::string &name) const;

private:
     ScopeManager scope_mgr;
     FunctionManager &function_manager;

     Value eval_binary_op(const std::string &op, const Value &lhs, const Value &rhs);
     Value eval_bool_op(const Value &lhs, const std::string &op, const Value &rhs);
     Value eval_string_op(const Value &lhs, const std::string &op, const Value &rhs);
     Value eval_number_op(const Value &lhs, const std::string &op, const Value &rhs);

     Value execute_for(const std::shared_ptr<ASTNode> &node);
     Value execute_for_loop(const std::shared_ptr<ASTNode> &body, const std::shared_ptr<ASTNode> &limit, const std::string &var_name);
     Value execute_while(const std::shared_ptr<ASTNode> &condition, const std::shared_ptr<ASTNode> &body);

     bool is_true(const Value &val) const;
};
