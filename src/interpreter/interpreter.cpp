#include "interpreter/interpreter.hpp"
#include "interpreter/builtins.hpp"

Interpreter::Interpreter()
    : evaluator(function_manager) 
{
     function_manager.register_native("cout", builtin_cout);
}

void Interpreter::interpret(const std::vector<std::shared_ptr<ASTNode>> &nodes)
{
     for (const auto &node : nodes)
     {
          evaluator.evaluate(node);
     }
}
