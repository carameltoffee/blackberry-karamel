#pragma once
#include <memory>
#include <vector>
#include "value.hpp"
#include "scope.hpp"
#include <parser.hpp>

class FunctionManager;

class Evaluator
{
public:
     Evaluator(FunctionManager &func_mgr);

     Value evaluate(const std::shared_ptr<ASTNode> &node);
     Value evaluate_block(const std::shared_ptr<ASTNode> &block);

     void push_scope();
     void pop_scope();
     Value get_variable(const std::string &name);
     void define_variable(const std::string &name, const Value &value);
     void set_variable(const std::string &name, const Value &value);

private:
     std::vector<Scope> scopes;
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
