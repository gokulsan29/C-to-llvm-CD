#include "ast.h"
#include "common.h"
#include "c_type.h"

c_type_t*
declaration_specifiers_n::declaration_specifiers_get_c_type() const
{
  vector<declaration_specifier_n*> decl_spec_v = this->get_list();
  c_type_t::base_type_t base_type = c_type_t::NO_TYPE;
  size_t num_long = 0;
  bool is_const = false;
  bool is_signed = false;
  bool is_unsigned = false;
  for (auto const& decl_spec : decl_spec_v) {
    if (decl_spec->is_base_type_specifier() && base_type != c_type_t::NO_TYPE) {
      return nullptr;
    }
    switch (decl_spec->get_declaration_specifier()) {
      case specifier_t::VOID: {
        base_type = c_type_t::VOID;
        break;
      }
      case specifier_t::CHAR: {
        base_type = c_type_t::CHAR;
        break;
      }
      case specifier_t::SHORT: {
        base_type = c_type_t::SHORT;
        break;
      }
      case specifier_t::INT: {
        base_type = c_type_t::INT;
        break;
      }
      case specifier_t::LONG: {
        if (num_long >= 2) {
          cout << "More than 2 \"long\" specifiers" << endl;
          return nullptr;
        }
        num_long++;
        break;
      }
      case specifier_t::FLOAT: {
        base_type = c_type_t::FLOAT;
        break;
      }
      case specifier_t::DOUBLE: {
        base_type = c_type_t::DOUBLE;
        break;
      }
      case specifier_t::SIGNED: {
        is_signed = true;
        break;
      }
      case specifier_t::UNSIGNED: {
        is_unsigned = true;
        break;
      }
      case specifier_t::CONST: {
        is_const = true;
        break;
      }
      default: {
        // Ideally this should only handle start and end specifer markers
        NOT_REACHED();
      }
    }
  }
  if (num_long > 0) {
    if (base_type == c_type_t::NO_TYPE) {
      base_type = (c_type_t::base_type_t)((int)c_type_t::INT + num_long);
    }
    else if (base_type == c_type_t::INT) {
      base_type = (c_type_t::base_type_t)((int)base_type + num_long);
    }
    else if (base_type == c_type_t::DOUBLE && num_long == 1) {
      base_type = c_type_t::LONG_DOUBLE;
    }
    else if (num_long == 1) {
      cout << "long " << c_type_t::base_type_to_string(base_type)
           << "is not a valid c type" << endl;
      return nullptr;
    }
    else if (num_long == 2) {
      cout << "long long " << c_type_t::base_type_to_string(base_type)
           << "is not a valid c type" << endl;
      return nullptr;
    }
    else {
      cout << __FUNCTION__ << "unknown error in getting c type" << endl;
      exit(1);
    }
  }
  if (is_signed && is_unsigned) {
    cout << "signed and unsigned keywords appearing together" << endl;
    return nullptr;
  }
  if (is_signed && base_type == c_type_t::NO_TYPE) {
    base_type = c_type_t::INT;
  }
  if (is_unsigned && base_type == c_type_t::NO_TYPE) {
    base_type = c_type_t::INT;
  }
  if (base_type == c_type_t::NO_TYPE) {
    cout << "base type not found using declaration specifiers" << endl;
    return nullptr;
  }
  if ((is_signed || is_unsigned) &&
      !c_type_t::base_type_can_have_sign_keywords(base_type)) {
    cout << c_type_t::base_type_to_string(base_type) << " does not take sign keywords" << endl;
    return nullptr;
  }
  return new c_type_t(base_type, is_const, is_signed, is_unsigned);
}

declaration_n::declaration_n(declaration_specifiers_n* declaration_specifiers,
                             init_declarator_list_n* init_declarator_list) :
  m_declaration_specifiers(declaration_specifiers),
  m_init_declarator_list(init_declarator_list)
{
  c_type_t* c_type = this->m_declaration_specifiers->declaration_specifiers_get_c_type();
  cout << c_type->c_type_to_string() << endl;
}

function_definition_n::function_definition_n(
    declaration_specifiers_n* declaration_specifiers,
    declarator_n* declarator,
    compound_statement_n* compound_statement) :
  m_declaration_specifiers(declaration_specifiers),
  m_declarator(declarator),
  m_compound_statement(compound_statement)
{
  c_type_t* c_type = this->m_declaration_specifiers->declaration_specifiers_get_c_type();
  cout << c_type->c_type_to_string() << endl;
}

parameter_declaration_n::parameter_declaration_n(
    declaration_specifiers_n* declaration_specifiers,
    declarator_n* declarator) :
  m_declaration_specifiers(declaration_specifiers),
  m_declarator(declarator)
{
  c_type_t* c_type = this->m_declaration_specifiers->declaration_specifiers_get_c_type();
  cout << c_type->c_type_to_string() << endl;
}

