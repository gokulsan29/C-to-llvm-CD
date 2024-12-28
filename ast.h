#pragma once
#include <string>
#include <vector>

using namespace std;

class ast_n
{
public:
  string to_string(string prefix="") const { return ""; }
};

template <typename T_NODE>
class list_n : public ast_n
{
public:
  string to_string(string prefix="") const;
  vector<T_NODE*> get_list() { return this->m_list; }
private:
  vector<T_NODE*> m_list;
};

class function_definition_n : public ast_n
{
public:
  string to_string(string prefix="") const;
};

class declaration_n : public ast_n
{
public:
  string to_string(string prefix="") const;
};

union external_declaration_n
{
  function_definition_n* m_f;
  declaration_n* m_d;
};

class translation_unit_n : public list_n<external_declaration_n>
{
public:
  string to_string(string prefix="") const;
};

