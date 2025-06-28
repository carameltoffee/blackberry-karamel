#include "interpreter/builtins.hpp"
#include <iostream>

Value builtin_cout(const std::vector<Value> &args)
{
     for (const auto &arg : args)
     {
          std::cout << arg.to_string();
     }
     std::cout << std::endl;
     return Value(); 
}
