#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include "value.hpp"

class ScopeManager
{
public:
     void push_scope();
     void pop_scope();

     bool has(const std::string &name) const;
     bool has_in_current(const std::string &name) const;

     void define(const std::string &name, const Value &value);
     void set(const std::string &name, const Value &value);
     Value get(const std::string &name) const;

private:
     std::vector<std::unordered_map<std::string, Value>> scopes;
};
