#pragma once
#include <vector>
#include <memory>
#include "evaluator.hpp"
#include "function_manager.hpp"

class Interpreter
{
public:
     Interpreter();

     void interpret(const std::vector<std::shared_ptr<ASTNode>> &nodes);

private:
     FunctionManager function_manager;
     Evaluator evaluator;
};
