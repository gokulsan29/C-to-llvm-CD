#pragma once
#include <iostream>
#include <string>
#include <vector>

#include "common.h"

using namespace std;

// Enum for declaration specifiers
enum class specifier {
  // storage class specifiers
  STORAGE_CLASS_SPECIFIER_START, TYPEDEF, EXTERN, STATIC, THREAD_LOCAL, AUTO, REGISTER, STORAGE_CLASS_SPECIFIER_END,
  // type specifiers
  TYPE_SPECIFIER_START, VOID, CHAR, SHORT, INT, LONG, FLOAT, DOUBLE, SIGNED, UNSIGNED, BOOL, COMPLEX, IMAGINARY, STRUCT, UNION, ENUM, TYPE_SPECIFIER_END,
  // type qualfiers
  TYPE_QUALIFIER_START, CONST, RESTRICT, VOLATILE, ATOMIC, TYPE_QUALIFIER_END,
  // function specifiers
  FUNCTION_SPECIFIER_START, INLINE, NORETURN, FUNCTION_SPECIFIER_END,
  // alignment Specifiers
  ALIGNAS
};

class ast_n
{
public:
  string to_string_ast(string prefix="") const { NOT_IMPLEMENTED(); }
};

template <typename T_NODE>
class list_n : public ast_n
{
public:
  list_n() : m_list(vector<T_NODE*>()) { }
  list_n(vector<T_NODE*> list) : m_list(list) { }

  string to_string_ast(string prefix="") const;
  vector<T_NODE*> const& get_list() const { return this->m_list; }
  void add_child(T_NODE* c) { this->m_list.push_back(c); }
  void add_child_front(T_NODE* c) { this->m_list.insert(this->m_list.begin(), c); }
private:
  vector<T_NODE*> m_list;
};

class string_n : public ast_n
{
public:
  string_n(string s) : m_str(s) { }
  string_n(char* s) { assert(s); m_str = string(s); }
  string to_string_ast(string prefix="") const;
  string const& get_str() const { return this->m_str; }
private:
  string m_str;
};

class declaration_specifier_n : public ast_n
{
public:
  declaration_specifier_n(specifier declaration_specifier) : m_declaration_specifier(declaration_specifier) { }
  string to_string_ast(string prefix="") const;
private:
  specifier m_declaration_specifier;
};

class declaration_specifiers_n : public list_n<declaration_specifier_n>
{
public:
  declaration_specifiers_n() : list_n<declaration_specifier_n>() { }
  declaration_specifiers_n(vector<declaration_specifier_n*> l) : list_n<declaration_specifier_n>(l) { }
  string to_string_ast(string prefix="") const;
};

class identifier_n : public string_n
{
public:
  identifier_n(char* s) : string_n(s) { }
  string to_string_ast(string prefix="") const;
  string const& get_identifier_name() const { return this->get_str(); }
};

class direct_declarator_item_n : public ast_n
{
public:
  enum item_opt_t
  {
    IDENTIFIER
  };
  string to_string_ast(string prefix="") const;
private:
  item_opt_t m_item_opt;
  union
  {
    identifier_n* id_node;
  } m_item;
};

class function_definition_n : public ast_n
{
public:
  function_definition_n(declaration_specifiers_n* declaration_specifiers) : m_declaration_specifiers(declaration_specifiers) { }
  string to_string_ast(string prefix="") const;
private:
  declaration_specifiers_n* m_declaration_specifiers;
};

class declaration_n : public ast_n
{
public:
  declaration_n(declaration_specifiers_n* declaration_specifiers) : m_declaration_specifiers(declaration_specifiers) { }
  string to_string_ast(string prefix="") const;
private:
  declaration_specifiers_n* m_declaration_specifiers;
};

class external_declaration_n : public ast_n
{
public:
  external_declaration_n(function_definition_n* function_definition) : m_function_definition(function_definition), m_declaration(nullptr) { }
  external_declaration_n(declaration_n* declaration) : m_function_definition(nullptr), m_declaration(declaration) { }
  string to_string_ast(string prefix="") const;
private:
  function_definition_n* m_function_definition;
  declaration_n* m_declaration;
};

class translation_unit_n : public list_n<external_declaration_n>
{
public:
  translation_unit_n() : list_n<external_declaration_n>() { }
  translation_unit_n(vector<external_declaration_n*> l) : list_n<external_declaration_n>(l) { }
  string to_string_ast(string prefix="") const;
};

