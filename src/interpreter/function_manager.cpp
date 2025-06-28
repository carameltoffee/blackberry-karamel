#include "interpreter/function_manager.hpp"
#include "interpreter/evaluator.hpp"
#include <iostream>
#include <stdexcept>

void FunctionManager::register_function(const std::string &name, const std::shared_ptr<ASTNode> &func_def)
{
     user_functions[name] = func_def;
}

void FunctionManager::register_native(const std::string &name, NativeFunc func)
{
     native_functions[name] = func;
}

std::vector<Value> FunctionManager::evaluate_args(const std::vector<std::shared_ptr<ASTNode>> &args, Evaluator &evaluator)
{
     std::vector<Value> evaluated;
     for (const auto &arg : args)
          evaluated.push_back(evaluator.evaluate(arg));
     return evaluated;
}

std::pair<std::string, std::string> extract_name_and_type(const std::string &s)
{
     auto pos = s.find(':');
     if (pos == std::string::npos || pos + 1 >= s.size())
          std::cout << s;
          std::exit(1);
          // throw std::runtime_error("Malformed type annotation: " + s);

     std::string name = s.substr(0, pos);
     std::string type = s.substr(pos + 1);
     return {name, type};
}

Value FunctionManager::call(const std::string &name, const std::vector<std::shared_ptr<ASTNode>> &args, Evaluator &evaluator)
{
     if (native_functions.find(name) != native_functions.end())
     {
          return native_functions.at(name)(evaluate_args(args, evaluator));
     }

     if (user_functions.find(name) == user_functions.end())
          throw std::runtime_error("Function not found: " + name);

     const auto &func_def = user_functions.at(name);
     auto &params_node = func_def->children[0];
     auto &body_node = func_def->children.back();

     if (params_node->children.size() != args.size())
          throw std::runtime_error("Argument count mismatch in function: " + name);

     evaluator.push_scope();
     try
     {
          for (size_t i = 0; i < args.size(); ++i)
          {
               auto [param_name, param_type] = extract_name_and_type(params_node->children[i]->value);
               Value arg_val = evaluator.evaluate(args[i]);

               Value::Type expected_type = Value::string_to_type(param_type);
               arg_val.check_type(expected_type);

               evaluator.define_variable(param_name, arg_val);
          }

          Value result = evaluator.evaluate_block(body_node);
          evaluator.pop_scope();
          return result;
     }
     catch (const ReturnSignal &ret)
     {
          evaluator.pop_scope();

          auto [_, ret_type]  = extract_name_and_type(func_def->children[1]->value);
          auto expected_type = Value::string_to_type(ret_type);
          ret.value.check_type(expected_type);

          return ret.value;
     }
     catch (...)
     {
          evaluator.pop_scope();
          throw;
     }
}