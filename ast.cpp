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

static const map<constant_n::constant_sort_t, string> constant_sort_to_str_map = {
  {constant_n::STRING_LITERAL, "string_literal"},
  {constant_n::INTEGER_CONST, "integer_constant"},
  {constant_n::FLOAT_CONST, "float_constant"},
};

static const map<expression_n::operation_kind_t, string> op_kind_to_str_map = {
  {expression_n::OP_EMPTY, "no-op"},
  {expression_n::OP_COMMA, "comma"},
  {expression_n::OP_FUNC_CALL, "function_call"},
  {expression_n::OP_FUNC_ARGS, "args"},
  {expression_n::OP_POST_INC, "post_inc"},
  {expression_n::OP_POST_DEC, "post_dec"},
  {expression_n::OP_PRE_INC, "pre_inc"},
  {expression_n::OP_PRE_DEC, "pre_dec"},
  {expression_n::OP_POS, "pos"},
  {expression_n::OP_NEG, "neg"},
  {expression_n::OP_COMPLEMENT, "~"},
  {expression_n::OP_LOGIC_NOT, "not"},
  {expression_n::OP_MUL, "mul"},
  {expression_n::OP_DIV, "div"},
  {expression_n::OP_MOD, "mod"},
  {expression_n::OP_ADD, "add"},
  {expression_n::OP_SUB, "sub"},
  {expression_n::OP_LSHIFT, "<<"},
  {expression_n::OP_RSHIFT, ">>"},
  {expression_n::OP_LT, "lt"},
  {expression_n::OP_GT, "gt"},
  {expression_n::OP_LTE, "lte"},
  {expression_n::OP_GTE, "gte"},
  {expression_n::OP_EQ, "eq"},
  {expression_n::OP_NEQ, "neq"},
  {expression_n::OP_BIT_AND, "&"},
  {expression_n::OP_BIT_OR, "|"},
  {expression_n::OP_XOR, "xor"},
  {expression_n::OP_LOGIC_AND, "and"},
  {expression_n::OP_LOGIC_OR, "or"},
  {expression_n::OP_CONDITIONAL, "cond"},
  {expression_n::OP_ASSIGN, "="},
  {expression_n::OP_MUL_ASSIGN, "*="},
  {expression_n::OP_DIV_ASSIGN, "/="},
  {expression_n::OP_MOD_ASSIGN, "%="},
  {expression_n::OP_ADD_ASSIGN, "+="},
  {expression_n::OP_SUB_ASSIGN, "-="},
  {expression_n::OP_LSHIFT_ASSIGN, "<<="},
  {expression_n::OP_RSHIFT_ASSIGN, ">>="},
  {expression_n::OP_BIT_AND_ASSIGN, "&="},
  {expression_n::OP_XOR_ASSIGN, "^="},
  {expression_n::OP_BIT_OR_ASSIGN, "|="},
};

static const map<jump_statement_n::jump_sort_t, string> jump_sort_to_str_map = {
  {jump_statement_n::RETURN, "return"},
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
constant_n::to_string_ast(string prefix) const
{
  return "constant " + constant_sort_to_str_map.at(this->get_sort()) + ": " + this->get_str();
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
    case direct_declarator_item_n::PARAMETER_LIST: {
      parameter_list_n* parameter_list = dynamic_cast<parameter_list_n*>(this->m_item);
      return parameter_list->to_string_ast(prefix);
    }
    default: {
      NOT_REACHED();
    }
  }
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
expression_n::to_string_ast(string prefix) const
{
  switch (this->get_kind()) {
    case expression_n::OP_VAR: {
      return this->get_identifier()->to_string_ast(prefix);
    }
    case expression_n::OP_CONST: {
      return this->get_constant()->to_string_ast(prefix);
    }
    default: break; // Non var and non const cases are handled below
  }
  string ret = op_kind_to_str_map.at(this->m_op_kind);
  if (this->get_size() != 0) {
    ret += this->list_n<expression_n>::to_string_ast(prefix);
  }
  return ret;
}

string
selection_statement_n::to_string_ast(string prefix) const
{
  string ret = "selection_statement";
  return ret;
}

string
iteration_statement_n::to_string_ast(string prefix) const
{
  string ret = "iteration_statement";
  return ret;
}

string
jump_statement_n::to_string_ast(string prefix) const
{
  string ret = jump_sort_to_str_map.at(this->get_jump_sort());
  if (this->get_jump_sort() == jump_statement_n::RETURN &&
      this->get_expr_for_return() != nullptr) {
    ret += "\n" + prefix + "`-" +
            this->get_expr_for_return()->to_string_ast(prefix + "  ");
  }
  return ret;
}

string
statement_n::to_string_ast(string prefix) const
{
  switch (this->m_statement_type) {
    case statement_n::COMPOUND_STATEMENT: {
      compound_statement_n* compound_statement =
        dynamic_cast<compound_statement_n*>(this->m_generic_statement);
      return compound_statement->to_string_ast(prefix);
    }
    case statement_n::EXPRESSION: {
      expression_n* expression = dynamic_cast<expression_n*>(this->m_generic_statement);
      return expression->to_string_ast(prefix);
    }
    case statement_n::SELECTION_STATEMENT: {
      selection_statement_n* selection_statement =
        dynamic_cast<selection_statement_n*>(this->m_generic_statement);
      return selection_statement->to_string_ast(prefix);
    }
    case statement_n::ITERATION_STATEMENT: {
      iteration_statement_n* iteration_statement =
        dynamic_cast<iteration_statement_n*>(this->m_generic_statement);
      return iteration_statement->to_string_ast(prefix);
    }
    case statement_n::JUMP_STATEMENT: {
      jump_statement_n* jump_statement =
        dynamic_cast<jump_statement_n*>(this->m_generic_statement);
      return jump_statement->to_string_ast(prefix);
    }
    default: {
      NOT_REACHED();
    }
  }
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

