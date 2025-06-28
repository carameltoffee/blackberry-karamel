#include "interpreter/scope_manager.hpp"
#include <stdexcept>

void ScopeManager::push_scope()
{
     scopes.emplace_back();
}

void ScopeManager::pop_scope()
{
     if (scopes.empty())
          throw std::runtime_error("No scopes to pop");
     scopes.pop_back();
}

bool ScopeManager::has(const std::string &name) const
{
     for (auto it = scopes.rbegin(); it != scopes.rend(); ++it)
     {
          if (it->find(name) != it->end())
               return true;
     }
     return false;
}

bool ScopeManager::has_in_current(const std::string &name) const
{
     if (scopes.empty())
          return false;
     return scopes.back().find(name) != scopes.back().end();
}

void ScopeManager::define(const std::string &name, const Value &value)
{
     if (scopes.empty())
          throw std::runtime_error("No active scope to define variable: " + name);

     auto &current = scopes.back();
     if (current.find(name) != current.end())
          throw std::runtime_error("Variable already defined in current scope: " + name);

     current[name] = value;
}

void ScopeManager::set(const std::string &name, const Value &value)
{
     for (auto it = scopes.rbegin(); it != scopes.rend(); ++it)
     {
          if (it->find(name) != it->end())
          {
               (*it)[name] = value;
               return;
          }
     }
     throw std::runtime_error("Undefined variable: " + name);
}

Value ScopeManager::get(const std::string &name) const
{
     for (auto it = scopes.rbegin(); it != scopes.rend(); ++it)
     {
          auto found = it->find(name);
          if (found != it->end())
               return found->second;
     }
     throw std::runtime_error("Undefined variable: " + name);
}
