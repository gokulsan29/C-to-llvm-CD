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

class constant_n : public string_n // Node for compile time constants, including enums
{
public:
  enum constant_sort_t
  {
    STRING_LITERAL,
    INTEGER_CONST,
    FLOAT_CONST
  };
  constant_n(constant_sort_t constant_sort, char* s) : string_n(s), m_sort(constant_sort) { }
  constant_sort_t get_sort() const { return this->m_sort; }
  string to_string_ast(string prefix="") const;
private:
  constant_sort_t m_sort;
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
    PARAMETER_LIST
  };
  direct_declarator_item_n(identifier_n* identifier) :
    m_item_opt(IDENTIFIER), m_item(identifier)
  { }
  direct_declarator_item_n(parameter_list_n* parameter_list) :
    m_item_opt(PARAMETER_LIST), m_item(parameter_list)
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

class statement_n;
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

class expression_n : public list_n<expression_n>
{
public:
  enum operation_kind_t
  {
    OP_EMPTY,
    OP_COMMA, // expr a and b are executed independently
    OP_VAR, // may be a var or function name
    OP_CONST,
    OP_FUNC_CALL,
    OP_FUNC_ARGS,
    OP_POST_INC,
    OP_POST_DEC,
    OP_PRE_INC,
    OP_PRE_DEC,
    OP_POS,
    OP_NEG,
    OP_COMPLEMENT,
    OP_LOGIC_NOT,
    OP_MUL,
    OP_DIV,
    OP_MOD,
    OP_ADD,
    OP_SUB,
    OP_LSHIFT,
    OP_RSHIFT,
    OP_LT,
    OP_GT,
    OP_LTE,
    OP_GTE,
    OP_EQ,
    OP_NEQ,
    OP_BIT_AND,
    OP_BIT_OR,
    OP_XOR,
    OP_LOGIC_AND,
    OP_LOGIC_OR,
    OP_CONDITIONAL,
    OP_ASSIGN,
    OP_MUL_ASSIGN,
    OP_DIV_ASSIGN,
    OP_MOD_ASSIGN,
    OP_ADD_ASSIGN,
    OP_SUB_ASSIGN,
    OP_LSHIFT_ASSIGN,
    OP_RSHIFT_ASSIGN,
    OP_BIT_AND_ASSIGN,
    OP_XOR_ASSIGN,
    OP_BIT_OR_ASSIGN,
  };
  expression_n() : list_n<expression_n>(), m_op_kind(OP_EMPTY) { }
  expression_n(identifier_n* identifier) : m_op_kind(OP_VAR), m_identifier(identifier) { }
  expression_n(constant_n* constant) : m_op_kind(OP_CONST), m_constant(constant) { }
  expression_n(operation_kind_t op, expression_n* e) :
    m_op_kind(op)
  {
    this->add_child(e);
  }
  expression_n(operation_kind_t op, expression_n* e1,  expression_n* e2) :
    m_op_kind(op)
  {
    this->add_child(e1);
    this->add_child(e2);
  }
  expression_n(operation_kind_t op, expression_n* e1,  expression_n* e2, expression_n* e3) :
    m_op_kind(op)
  {
    this->add_child(e1);
    this->add_child(e2);
    this->add_child(e3);
  }
  operation_kind_t get_kind() const { return this->m_op_kind; }
  bool is_var() const { return this->get_kind() == OP_VAR; }
  bool is_const() const { return this->get_kind() == OP_CONST; }
  identifier_n const* get_identifier() const { assert(this->is_var()); return this->m_identifier; }
  constant_n const* get_constant() const { assert(this->is_const()); return this->m_constant; }
  string to_string_ast(string prefix="") const;

  static expression_n* mk_func_args()
  {
    return new expression_n(OP_FUNC_ARGS);
  }

  static expression_n* mk_func_args(vector<expression_n*> args)
  {
    return new expression_n(OP_FUNC_ARGS, args);
  }
private:
  expression_n(operation_kind_t op_kind) : m_op_kind(op_kind) { }
  expression_n(operation_kind_t op_kind, vector<expression_n*> expr_vec) :
    m_op_kind(op_kind), list_n<expression_n>(expr_vec)
  { }
  operation_kind_t m_op_kind;
  identifier_n* m_identifier;
  constant_n* m_constant;
};

class selection_statement_n : public ast_n
{
public:
  enum selection_sort_t
  {
    IF_THEN,
    IF_THEN_ELSE,
  };
  selection_statement_n(selection_sort_t sort, expression_n* cond, statement_n* body) :
    m_sort(sort), m_cond(cond), m_body(body)
  { }
  selection_statement_n(expression_n* cond, statement_n* body, statement_n* else_body) :
    m_sort(IF_THEN_ELSE), m_cond(cond), m_body(body), m_else_body(else_body)
  { }

  selection_sort_t get_selection_sort() const { return this->m_sort; }
  expression_n const* get_cond() const { return this->m_cond; }
  statement_n const* get_body() const { return this->m_body; }
  statement_n const* get_else_body() const
  {
    assert(this->get_selection_sort() == IF_THEN_ELSE);
    return this->m_else_body;
  }
  string to_string_ast(string prefix="") const;
private:
  selection_sort_t m_sort;
  expression_n* m_cond;
  statement_n* m_body;
  statement_n* m_else_body = nullptr;
};

class iteration_statement_n : public ast_n
{
public:
  enum iteration_sort_t
  {
    WHILE,
    DO_WHILE,
    FOR,
    FOR_DECL,
  };

  iteration_sort_t get_iteration_sort() const { return this->m_sort; }
  expression_n const* get_cond() const { return this->m_cond; }
  statement_n const* get_body() const { return this->m_body; }
  expression_n const* get_init_expr() const
  {
    assert(this->get_iteration_sort() == FOR);
    return this->m_init_expr;
  }
  declaration_n const* get_init_decl() const
  {
    assert(this->get_iteration_sort() == FOR_DECL);
    return this->m_init_decl;
  }
  expression_n const* get_update_expr() const
  {
    assert(this->get_iteration_sort() == FOR || this->get_iteration_sort() == FOR_DECL);
    return this->m_update;
  }
  bool iteration_statement_has_update_expr() const
  {
    return (this->get_iteration_sort() == FOR || this->get_iteration_sort() == FOR_DECL) &&
           this->m_update != nullptr;
  }
  string to_string_ast(string prefix="") const;

  static iteration_statement_n* mk_while_iteration_statement(expression_n* cond, statement_n* body)
  {
    return new iteration_statement_n(WHILE, cond, body);
  }
  static iteration_statement_n* mk_do_while_iteration_statement(statement_n* body, expression_n* cond)
  {
    return new iteration_statement_n(DO_WHILE, cond, body);
  }
  static iteration_statement_n* mk_for_iteration_statement(expression_n* init_expr,
                                                           expression_n* cond,
                                                           statement_n* body)
  {
    return new iteration_statement_n(FOR, init_expr, cond, body);
  }
  static iteration_statement_n* mk_for_iteration_statement(expression_n* init_expr,
                                                           expression_n* cond,
                                                           expression_n* update,
                                                           statement_n* body)
  {
    return new iteration_statement_n(FOR, init_expr, cond, update, body);
  }
  static iteration_statement_n* mk_for_iteration_statement(declaration_n* init_decl,
                                                           expression_n* cond,
                                                           statement_n* body)
  {
    return new iteration_statement_n(FOR_DECL, init_decl, cond, body);
  }
  static iteration_statement_n* mk_for_iteration_statement(declaration_n* init_decl,
                                                           expression_n* cond,
                                                           expression_n* update,
                                                           statement_n* body)
  {
    return new iteration_statement_n(FOR_DECL, init_decl, cond, update, body);
  }
private:
  iteration_statement_n(iteration_sort_t sort, expression_n* cond, statement_n* body) :
    m_sort(sort), m_cond(cond), m_body(body)
  { }
  iteration_statement_n(iteration_sort_t sort,
                        expression_n* init_expr,
                        expression_n* cond,
                        statement_n* body) :
    m_sort(sort), m_init_expr(init_expr), m_cond(cond), m_body(body)
  { }
  iteration_statement_n(iteration_sort_t sort,
                        expression_n* init_expr,
                        expression_n* cond,
                        expression_n* update,
                        statement_n* body) :
    m_sort(sort), m_init_expr(init_expr), m_cond(cond), m_update(update), m_body(body)
  { }
  iteration_statement_n(iteration_sort_t sort,
                        declaration_n* init_decl,
                        expression_n* cond,
                        statement_n* body) :
    m_sort(sort), m_init_decl(init_decl), m_cond(cond), m_body(body)
  { }
  iteration_statement_n(iteration_sort_t sort,
                        declaration_n* init_decl,
                        expression_n* cond,
                        expression_n* update,
                        statement_n* body) :
    m_sort(sort), m_init_decl(init_decl), m_cond(cond), m_update(update), m_body(body)
  { }

  iteration_sort_t m_sort;
  expression_n* m_init_expr;
  declaration_n* m_init_decl;
  expression_n* m_cond;
  expression_n* m_update = nullptr;
  statement_n* m_body;
};

class jump_statement_n : public ast_n
{
public:
  enum jump_sort_t
  {
    RETURN,
  };
  jump_statement_n(jump_sort_t sort) : m_sort(sort) { }
  jump_statement_n(jump_sort_t sort, expression_n* expr) :
    m_sort(sort), m_expr(expr)
  { }

  jump_sort_t get_jump_sort() const { return this->m_sort; }
  expression_n const* get_expr_for_return() const
  {
    assert(this->get_jump_sort() == RETURN);
    return this->m_expr;
  }
  string to_string_ast(string prefix="") const;
private:
  jump_sort_t m_sort;
  expression_n* m_expr = nullptr;
};

class statement_n : public ast_n
{
public:
  enum statement_type_t
  {
    COMPOUND_STATEMENT,
    EXPRESSION,
    SELECTION_STATEMENT,
    ITERATION_STATEMENT,
    JUMP_STATEMENT
  };
  statement_n(compound_statement_n* compound_statement) :
    m_statement_type(COMPOUND_STATEMENT),
    m_generic_statement(compound_statement)
  { }
  statement_n(expression_n* expression) :
    m_statement_type(EXPRESSION),
    m_generic_statement(expression)
  { }
  statement_n(selection_statement_n* selection_statement) :
    m_statement_type(SELECTION_STATEMENT),
    m_generic_statement(selection_statement)
  { }
  statement_n(iteration_statement_n* iteration_statement) :
    m_statement_type(ITERATION_STATEMENT),
    m_generic_statement(iteration_statement)
  { }
  statement_n(jump_statement_n* jump_statement) :
    m_statement_type(JUMP_STATEMENT),
    m_generic_statement(jump_statement)
  { }
  string to_string_ast(string prefix="") const;
private:
  statement_type_t m_statement_type;
  ast_n* m_generic_statement;
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

