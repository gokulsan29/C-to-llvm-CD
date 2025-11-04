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
    ret += "\n" + prefix + elem_prefix + l[i]->to_string_ast(prefix + child_prefix);
  }
  return ret;
}

string
identifier_n::to_string_ast(string prefix) const
{
  return "identifier: " + this->get_str();
}

string
declaration_specifier_n::to_string_ast(string prefix) const
{
  specifier decl_spec = this->get_declaration_specifier();
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
  return specifier_to_str_map.at(decl_spec);
}

string
declaration_specifiers_n::to_string_ast(string prefix) const
{
  string ret = "";
  vector<declaration_specifier_n*> decl_spec_v = this->get_list();
  for (auto const decl_spec : decl_spec_v) {
    ret += decl_spec->to_string_ast(prefix) + " ";
  }
  return ret;
}

string
declarator_n::to_string_ast(string prefix) const
{
  string ret = "declarator";
  bool has_pointer = this->m_pointer != nullptr;
  if (has_pointer) {
    ret += "\n" + prefix + "|-" + this->m_pointer->to_string_ast(prefix + "| ");
  }
  ret += "\n" + prefix + "`-" +
         this->m_direct_declarator->to_string_ast(prefix + "  ");
  return ret;
}

string
initializer_n::to_string_ast(string prefix) const
{
  string ret = "initializer";
  return ret;
}

string
direct_declarator_item_n::to_string_ast(string prefix) const
{
  switch (this->m_item_opt) {
    case direct_declarator_item_n::IDENTIFIER: {
      identifier_n* identifier = dynamic_cast<identifier_n*>(this->m_item);
      return identifier->to_string_ast(prefix);
    }
    case direct_declarator_item_n::PARMETER_LIST: {
      parameter_list_n* parameter_list = dynamic_cast<parameter_list_n*>(this->m_item);
      return parameter_list->to_string_ast(prefix);
    }
    default: {
      NOT_REACHED();
    }
  }
  return "";
}

string
parameter_declaration_n::to_string_ast(string prefix) const
{
  string ret = "parameter_declaration";
  bool has_declarator = this->m_declarator != nullptr;
  string prefix_for_decl_spec = has_declarator ? "|-" : "`-";
  string child_prefix_for_decl_spec = has_declarator ? "| " : "  ";
  ret += "\n" + prefix + prefix_for_decl_spec +
         this->m_declaration_specifiers->to_string_ast(prefix + child_prefix_for_decl_spec);
  if (has_declarator) {
    ret += "\n" + prefix + "`-" + this->m_declarator->to_string_ast(prefix + "  ");
  }
  return ret;
}

string
parameter_list_n::to_string_ast(string prefix) const
{
  string ret = "parameter_list is_vararg: " + string(this->get_is_vararg() ? "true" : "false")
               + " (size: " + std::to_string(this->get_size()) + ")";
  ret += this->list_n<parameter_declaration_n>::to_string_ast(prefix);
  return ret;
}

string
direct_declarator_n::to_string_ast(string prefix) const
{
  string ret = "direct_declarator";
  ret += this->list_n<direct_declarator_item_n>::to_string_ast(prefix);
  return ret;
}

string
pointer_n::to_string_ast(string prefix) const
{
  string ret = "pointer: ";
  vector<declaration_specifiers_n*> decl_specs_v = this->get_list();
  for (auto const decl_specs : decl_specs_v) {
    ret += "* " + (decl_specs == nullptr ? "" : decl_specs->to_string_ast(prefix));
  }
  return ret;
}

string
init_declarator_n::to_string_ast(string prefix) const
{
  string ret = "init_declarator";
  bool has_initializer = this->m_initializer != nullptr;
  string prefix_for_declarator = has_initializer ? "|-" : "`-";
  string child_prefix_for_declarator = has_initializer ? "| " : "  ";
  ret += "\n" + prefix + prefix_for_declarator +
         this->m_declarator->to_string_ast(prefix + child_prefix_for_declarator);
  if (has_initializer) {
    ret += "\n" + prefix + "`-" + this->m_initializer->to_string_ast(prefix + "  ");
  }
  return ret;
}

string
init_declarator_list_n::to_string_ast(string prefix) const
{
  string ret = "init_declarator_list (size: " + std::to_string(this->get_size()) + ")";
  ret += this->list_n<init_declarator_n>::to_string_ast(prefix);
  return ret;
}

string
statement_n::to_string_ast(string prefix) const
{
  string ret = "statement";
  return ret;
}

string
block_item_n::to_string_ast(string prefix) const
{
  if (this->m_declaration) {
    return this->m_declaration->to_string_ast(prefix);
  }
  assert(this->m_statement);
  return this->m_statement->to_string_ast(prefix);
}

string
compound_statement_n::to_string_ast(string prefix) const
{
  string ret = "compound_statement";
  if (this->get_size() == 0) {
    ret += " EMPTY";
    return ret;
  }
  ret += this->list_n<block_item_n>::to_string_ast(prefix);
  return ret;
}

string
function_definition_n::to_string_ast(string prefix) const
{
  string ret = "function_definition";
  ret += "\n" + prefix + "|-" + this->m_declaration_specifiers->to_string_ast(prefix + "| ");
  ret += "\n" + prefix + "|-" + this->m_declarator->to_string_ast(prefix + "| ");
  ret += "\n" + prefix + "`-" + this->m_compound_statement->to_string_ast(prefix + "  ");
  return ret;
}

string
declaration_n::to_string_ast(string prefix) const
{
  string ret = "declaration";
  bool has_init_declarators = this->m_init_declarator_list != nullptr;
  string prefix_for_decl_spec = has_init_declarators ? "|-" : "`-";
  string child_prefix_for_decl_spec = has_init_declarators ? "| " : "  ";
  ret += "\n" + prefix + prefix_for_decl_spec +
         this->m_declaration_specifiers->to_string_ast(prefix + child_prefix_for_decl_spec);
  if (has_init_declarators) {
    ret += "\n" + prefix + "`-" + this->m_init_declarator_list->to_string_ast(prefix + "  ");
  }
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
  string ret = "translation_unit (size: " + std::to_string(this->get_size()) + ")";
  ret += list_n<external_declaration_n>::to_string_ast(prefix);
  return ret;
}

