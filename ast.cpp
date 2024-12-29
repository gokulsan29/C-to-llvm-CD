#include <string>
#include <vector>

#include "ast.h"

using namespace std;

template <typename T_NODE>
string
list_n<T_NODE>::to_string(string prefix) const
{
  string ret = "";
  vector<T_NODE*> l = this->get_list();
  for (int i = 0;i < l.size();i++) {
    string elem_prefix = i == l.size() - 1 ? "`-" : "|-";
    ret += prefix + elem_prefix + l[i]->to_string(prefix);
  }
  return ret;
}

string
external_declaration_n::to_string(string prefix) const
{
  if (this->m_f != nullptr) {
    return this->m_f->to_string(prefix);
  }
  else {
    return this->m_d->to_string(prefix);
  }
}

string
translation_unit_n::to_string(string prefix) const
{
  string ret = "";
  ret += "translation_unit\n";
  ret += list_n<external_declaration_n>::to_string(prefix + " ");
  return ret;
}

string
function_definition_n::to_string(string prefix) const
{
  string ret = "";
  ret += "function_definition\n";
  return ret;
}

string
declaration_n::to_string(string prefix) const
{
  string ret = "";
  ret += "declaration\n";
  return ret;
}

