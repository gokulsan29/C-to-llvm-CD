#pragma once
#include <cassert>
#include <string>
#include <vector>

using namespace std;

// Enum for declaration specifiers
enum class spec_const {
  // storage class specifiers
  TYPEDEF = 0, EXTERN, STATIC, THREAD_LOCAL, AUTO, REGISTER,
  // type specifiers
  VOID = 10, CHAR, SHORT, INT, LONG, FLOAT, DOUBLE, SIGNED, UNSIGNED, BOOL, COMPLEX, IMAGINARY, ATOMIC,
  STRUCT, UNION, ENUM
};

class ast_n
{
public:
  string to_string(string prefix="") const { return ""; }
};

template <typename T_NODE>
class list_n : public ast_n
{
public:
  list_n() : m_list(vector<T_NODE*>()) { }
  list_n(vector<T_NODE*> list) : m_list(list) { }

  string to_string(string prefix="") const;
  vector<T_NODE*> get_list() const { return this->m_list; }
  void add_child(T_NODE* c) { return this->m_list.push_back(c); }
private:
  vector<T_NODE*> m_list;
};

class function_definition_n : public ast_n
{
public:
  function_definition_n() { }
  string to_string(string prefix="") const;
};

class declaration_n : public ast_n
{
public:
  declaration_n() { }
  string to_string(string prefix="") const;
};

class external_declaration_n : public ast_n
{
public:
  external_declaration_n(function_definition_n* f, declaration_n* d)
  {
    assert(f == nullptr || d == nullptr);
    assert(f != nullptr || d != nullptr);
    this->m_f = f;
    this->m_d = d;
  }
  string to_string(string prefix="") const;
private:
  function_definition_n* m_f;
  declaration_n* m_d;
};

class translation_unit_n : public list_n<external_declaration_n>
{
public:
  translation_unit_n() : list_n<external_declaration_n>() { }
  translation_unit_n(vector<external_declaration_n*> l) : list_n<external_declaration_n>(l) { }
  string to_string(string prefix="") const;
};

