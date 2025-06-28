#pragma once
#include <string>
#include <vector>
#include <variant>

class Value
{
public:
     enum class Type
     {
          None,
          Int,
          Float,
          String,
          Bool,
          Array
     };

     Value();
     Value(int);
     Value(double);
     Value(const std::string &);
     Value(bool);
     Value(const std::vector<Value> &);

     Type type;

     int int_val;
     double float_val;
     std::string str_val;
     bool bool_val;
     std::vector<Value> array_val;

     bool is_number() const;
     std::string to_string() const;
     void check_type(Type expected) const;
     static Type string_to_type(const std::string &type_str);
};
