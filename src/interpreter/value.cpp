#include "interpreter/value.hpp"
#include <stdexcept>

Value::Value() : type(Type::None), int_val(0), float_val(0.0), bool_val(false) {}
Value::Value(int v) : type(Type::Int), int_val(v), float_val(0.0), bool_val(false) {}
Value::Value(double v) : type(Type::Float), int_val(0), float_val(v), bool_val(false) {}
Value::Value(const std::string &v) : type(Type::String), int_val(0), float_val(0.0), str_val(v), bool_val(false) {}
Value::Value(bool v) : type(Type::Bool), int_val(0), float_val(0.0), bool_val(v) {}
Value::Value(const std::vector<Value> &v) : type(Type::Array), int_val(0), float_val(0.0), bool_val(false), array_val(v) {}

bool Value::is_number() const
{
     return type == Type::Int || type == Type::Float;
}

std::string Value::to_string() const
{
     switch (type)
     {
     case Type::Int:
          return std::to_string(int_val);
     case Type::Float:
          return std::to_string(float_val);
     case Type::Bool:
          return bool_val ? "true" : "false";
     case Type::String:
          return str_val;
     case Type::Array:
     {
          std::string result = "[";
          for (size_t i = 0; i < array_val.size(); ++i)
          {
               result += array_val[i].to_string();
               if (i < array_val.size() - 1)
                    result += ", ";
          }
          result += "]";
          return result;
     }
     default:
          return "null";
     }
}

void Value::check_type(Type expected) const
{
     if (type != expected)
     {
          throw std::runtime_error("Type mismatch. Expected: " + std::to_string((int)expected) + ", got: " + std::to_string((int)type));
     }
}

Value::Type Value::string_to_type(const std::string &type_str)
{
     if (type_str == "num")
          return Type::Int;
     if (type_str == "flo")
          return Type::Float;
     if (type_str == "bool")
          return Type::Bool;
     if (type_str == "str")
          return Type::String;
     if (type_str == "arr")
          return Type::Array;
     throw std::runtime_error("Unknown type: " + type_str);
}

bool Value::is_array() const
{
     return type == Type::Array;
}
