#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include <vector>
#include <functional>
#include "value.hpp"

class Evaluator;
class ASTNode;

class FunctionManager
{
public:
     using NativeFunc = std::function<Value(const std::vector<Value> &)>;

     void register_function(const std::string &name, const std::shared_ptr<ASTNode> &func_def);
     void register_native(const std::string &name, NativeFunc func);
     Value call(const std::string &name, const std::vector<std::shared_ptr<ASTNode>> &args, Evaluator &evaluator);

private:
     std::unordered_map<std::string, std::shared_ptr<ASTNode>> user_functions;
     std::unordered_map<std::string, NativeFunc> native_functions;

     std::vector<Value> evaluate_args(const std::vector<std::shared_ptr<ASTNode>> &args, Evaluator &evaluator);
};
