#pragma once
#include <unordered_map>
#include <string>
#include <memory>
#include <vector>
#include "value.hpp"
#include "evaluator.hpp"

class FunctionManager
{
public:
     void register_function(const std::string &name, std::shared_ptr<ASTNode> decl);
     Value call(const std::string &name, const std::vector<std::shared_ptr<ASTNode>> &args, Evaluator &evaluator);

private:
     std::unordered_map<std::string, std::shared_ptr<ASTNode>> functions;

     bool matches_type(const Value &val, const std::string &expected_type);
     std::pair<std::string, std::string> extract_name_and_type(const std::string &s);
};
