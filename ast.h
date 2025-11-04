#pragma once

#include <stddef.h>
#include <string>
#include <vector>

#include "common.h"

using namespace std;

// Enum for declaration specifiers
enum class specifier {
  // storage class specifiers
  STORAGE_CLASS_SPECIFIER_START, TYPEDEF, EXTERN, STATIC, THREAD_LOCAL, AUTO, REGISTER,
  STORAGE_CLASS_SPECIFIER_END,
  // type specifiers
  TYPE_SPECIFIER_START, VOID, CHAR, SHORT, INT, LONG, FLOAT, DOUBLE, SIGNED, UNSIGNED, BOOL,
  COMPLEX, IMAGINARY, STRUCT, UNION, ENUM, TYPE_SPECIFIER_END,
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
  virtual ~ast_n() { }
};

template <typename T_NODE>
class list_n : public ast_n
{
public:
  list_n() : m_list(vector<T_NODE*>()) { }
  list_n(vector<T_NODE*> list) : m_list(list) { }

  string to_string_ast(string prefix="") const;
  size_t get_size() const { return this->m_list.size(); }
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
  declaration_specifier_n(specifier declaration_specifier) :
    m_declaration_specifier(declaration_specifier)
  { }
  string to_string_ast(string prefix="") const;

  specifier get_declaration_specifier() const { return m_declaration_specifier; }
  bool is_type_qualifier() {
    return m_declaration_specifier > specifier::TYPE_QUALIFIER_START &&
           m_declaration_specifier < specifier::TYPE_QUALIFIER_END;
  }
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

class declarator_n;
class parameter_declaration_n : public ast_n
{
public:
  parameter_declaration_n(declaration_specifiers_n* declaration_specifiers) :
    m_declaration_specifiers(declaration_specifiers),
    m_declarator(nullptr)
  { }
  parameter_declaration_n(declaration_specifiers_n* declaration_specifiers,
                          declarator_n* declarator) :
    m_declaration_specifiers(declaration_specifiers),
    m_declarator(declarator)
  { }
  string to_string_ast(string prefix="") const;
private:
  declaration_specifiers_n* m_declaration_specifiers;
  declarator_n* m_declarator;
};

class parameter_list_n : public list_n<parameter_declaration_n>
{
public:
  parameter_list_n() : list_n<parameter_declaration_n>() { }
  parameter_list_n(vector<parameter_declaration_n*> l) : list_n<parameter_declaration_n>(l) { }

  bool get_is_vararg() const { return m_is_vararg; }
  void set_is_vararg(bool is_vararg) { m_is_vararg = is_vararg; }
  string to_string_ast(string prefix="") const;
private:
  bool m_is_vararg = false;
};


class direct_declarator_item_n : public ast_n
{
public:
  enum item_opt_t
  {
    IDENTIFIER,
    PARMETER_LIST
  };
  direct_declarator_item_n(identifier_n* identifier) :
    m_item_opt(IDENTIFIER), m_item(identifier)
  { }
  direct_declarator_item_n(parameter_list_n* parameter_list) :
    m_item_opt(PARMETER_LIST), m_item(parameter_list)
  { }
  string to_string_ast(string prefix="") const;
private:
  item_opt_t m_item_opt;
  ast_n* m_item;
};

class direct_declarator_n : public list_n<direct_declarator_item_n>
{
public:
  direct_declarator_n() : list_n<direct_declarator_item_n>() { }
  direct_declarator_n(vector<direct_declarator_item_n*> l) : list_n<direct_declarator_item_n>(l) { }
  string to_string_ast(string prefix="") const;
};

class pointer_n : public list_n<declaration_specifiers_n>
{
public:
  pointer_n() : list_n<declaration_specifiers_n>() { }
  pointer_n(vector<declaration_specifiers_n*> l) : list_n<declaration_specifiers_n>(l) { }
  string to_string_ast(string prefix="") const;
};

class declarator_n : public ast_n
{
public:
  declarator_n(pointer_n* pointer, direct_declarator_n* direct_declarator) :
    m_pointer(pointer),
    m_direct_declarator(direct_declarator)
  { }
  declarator_n(direct_declarator_n* direct_declarator) :
    m_pointer(nullptr),
    m_direct_declarator(direct_declarator)
  { }
  string to_string_ast(string prefix="") const;
private:
  pointer_n* m_pointer;
  direct_declarator_n* m_direct_declarator;
};

class initializer_n : public ast_n
{
public:
  string to_string_ast(string prefix="") const;
};

class init_declarator_n : public ast_n
{
public:
  init_declarator_n(declarator_n* declarator) :
    m_declarator(declarator),
    m_initializer(nullptr)
  { }
  init_declarator_n(declarator_n* declarator, initializer_n* initializer) :
    m_declarator(declarator),
    m_initializer(initializer)
  { }
  string to_string_ast(string prefix="") const;
private:
  declarator_n* m_declarator;
  initializer_n* m_initializer;
};

class init_declarator_list_n : public list_n<init_declarator_n>
{
public:
  init_declarator_list_n() : list_n<init_declarator_n>() { }
  init_declarator_list_n(vector<init_declarator_n*> l) : list_n<init_declarator_n>(l) { }
  string to_string_ast(string prefix="") const;
};

class statement_n : public ast_n
{
public:
  statement_n() { }
  string to_string_ast(string prefix="") const;
};

class declaration_n : public ast_n
{
public:
  declaration_n(declaration_specifiers_n* declaration_specifiers) :
    m_declaration_specifiers(declaration_specifiers),
    m_init_declarator_list(nullptr)
  { }
  declaration_n(declaration_specifiers_n* declaration_specifiers,
                init_declarator_list_n* init_declarator_list) :
    m_declaration_specifiers(declaration_specifiers),
    m_init_declarator_list(init_declarator_list)
  { }
  string to_string_ast(string prefix="") const;
private:
  declaration_specifiers_n* m_declaration_specifiers;
  init_declarator_list_n* m_init_declarator_list;
};

class block_item_n : public ast_n
{
public:
  block_item_n(declaration_n* declaration) : m_declaration(declaration), m_statement(nullptr) { }
  block_item_n(statement_n* statement) : m_declaration(nullptr), m_statement(statement) { }
  string to_string_ast(string prefix="") const;
private:
  declaration_n* m_declaration;
  statement_n* m_statement;
};

class compound_statement_n : public list_n<block_item_n>
{
public:
  compound_statement_n() : list_n<block_item_n>() { }
  compound_statement_n(vector<block_item_n*> l) : list_n<block_item_n>(l) { }
  string to_string_ast(string prefix="") const;
};

class function_definition_n : public ast_n
{
public:
  function_definition_n(declaration_specifiers_n* declaration_specifiers,
                        declarator_n* declarator,
                        compound_statement_n* compound_statement) :
    m_declaration_specifiers(declaration_specifiers),
    m_declarator(declarator),
    m_compound_statement(compound_statement)
  { }
  string to_string_ast(string prefix="") const;
private:
  declaration_specifiers_n* m_declaration_specifiers;
  declarator_n* m_declarator;
  compound_statement_n* m_compound_statement;
};

class external_declaration_n : public ast_n
{
public:
  external_declaration_n(function_definition_n* function_definition) :
    m_function_definition(function_definition),
    m_declaration(nullptr)
  { }
  external_declaration_n(declaration_n* declaration) :
    m_function_definition(nullptr),
    m_declaration(declaration)
  { }
  string to_string_ast(string prefix="") const;
private:
  function_definition_n* m_function_definition;
  declaration_n* m_declaration;
};

class translation_unit_n : public list_n<external_declaration_n>
{
public:
  translation_unit_n() : list_n<external_declaration_n>() { }
  translation_unit_n(vector<external_declaration_n*> l) :
    list_n<external_declaration_n>(l)
  { }
  string to_string_ast(string prefix="") const;
};

