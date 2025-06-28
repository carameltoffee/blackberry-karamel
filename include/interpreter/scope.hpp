#pragma once
#include <unordered_map>
#include <string>
#include "value.hpp"

class Scope
{
public:
     void define(const std::string &name, const Value &value);
     void assign(const std::string &name, const Value &value);
     Value get(const std::string &name) const;
     bool has(const std::string &name) const;

private:
     std::unordered_map<std::string, Value> variables;
};
