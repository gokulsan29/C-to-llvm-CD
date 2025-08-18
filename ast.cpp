#include <map>
#include <string>
#include <vector>

#include "ast.h"
#include "common.h"

using namespace std;

static const map<specifier, string> specifier_to_str_map = {
  // storage class specifiers
  {specifier::TYPEDEF, "typedef"},
  {specifier::EXTERN, "extern"},
  {specifier::STATIC, "static"},
  {specifier::THREAD_LOCAL, "thread_local"},
  {specifier::AUTO, "auto"},
  {specifier::REGISTER, "register"},
  // type specifiers
  {specifier::VOID, "void"},
  {specifier::CHAR, "char"},
  {specifier::SHORT, "short"},
  {specifier::INT, "int"},
  {specifier::LONG, "long"},
  {specifier::FLOAT, "float"},
  {specifier::DOUBLE, "double"},
  {specifier::SIGNED, "signed"},
  {specifier::UNSIGNED, "unsigned"},
  {specifier::BOOL, "bool"},
  {specifier::COMPLEX, "complex"},
  {specifier::IMAGINARY, "imaginary"},
  {specifier::STRUCT, "struct"},
  {specifier::UNION, "union"},
  {specifier::ENUM, "enum"},
  // type qualfiers
  {specifier::CONST, "const"},
  {specifier::RESTRICT, "restrict"},
  {specifier::VOLATILE, "volatile"},
  {specifier::ATOMIC, "atomic"},
  // function specifiers
  {specifier::INLINE, "inline"},
  {specifier::NORETURN, "noreturn"},
  // alignment specifier
  {specifier::ALIGNAS, "alignas"},
};

template <typename T_NODE>
string
list_n<T_NODE>::to_string_ast(string prefix) const
{
  string ret = "";
  vector<T_NODE*> const& l = this->get_list();
  for (int i = 0;i < l.size();i++) {
    string elem_prefix = i == l.size() - 1 ? "`-" : "|-";
    string child_prefix = i == l.size() - 1 ? "  " : "| ";
    ret += prefix + elem_prefix + l[i]->to_string_ast(prefix + child_prefix);
  }
  return ret;
} 

string
declaration_specifier_n::to_string_ast(string prefix) const
{
  specifier decl_spec = this->m_declaration_specifier;
  if (decl_spec == specifier::STORAGE_CLASS_SPECIFIER_START ||
      decl_spec == specifier::STORAGE_CLASS_SPECIFIER_END ||
      decl_spec == specifier::TYPE_SPECIFIER_START ||
      decl_spec == specifier::TYPE_SPECIFIER_END ||
      decl_spec == specifier::TYPE_QUALIFIER_START ||
      decl_spec == specifier::TYPE_QUALIFIER_END ||
      decl_spec == specifier::FUNCTION_SPECIFIER_START ||
      decl_spec == specifier::FUNCTION_SPECIFIER_END) {
    NOT_REACHED();
  }
  return specifier_to_str_map.at(this->m_declaration_specifier);
}

string
declaration_specifiers_n::to_string_ast(string prefix) const
{
  string ret = prefix + "declaration_specifiers: ";
  vector<declaration_specifier_n*> const& decl_spec_list = this->get_list();
  for(int i = 0;i < decl_spec_list.size();i++) {
    ret += decl_spec_list[i]->to_string_ast(prefix);
    if (i < decl_spec_list.size() - 1) {
      ret += " ";
    }
  }
  ret += "\n";
  return ret;
}

string
external_declaration_n::to_string_ast(string prefix) const
{
  if (this->m_function_definition) {
    return this->m_function_definition->to_string_ast(prefix);
  }
  assert(this->m_declaration);
  return this->m_declaration->to_string_ast(prefix);
}

string
translation_unit_n::to_string_ast(string prefix) const
{
  string ret = "translation_unit\n";
  ret += list_n<external_declaration_n>::to_string_ast(prefix + " ");
  return ret;
}

string
function_definition_n::to_string_ast(string prefix) const
{
  string ret = "function_definition\n";
  ret += prefix + this->m_declaration_specifiers->to_string_ast("|-");
  return ret;
}

string
declaration_n::to_string_ast(string prefix) const
{
  string ret = "declaration\n";
  ret += prefix + this->m_declaration_specifiers->to_string_ast("|-");
  return ret;
}

