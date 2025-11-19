#include <map>

#include "c_type.h"

static const map<c_type_t::base_type_t, string> base_type_to_str_map = {
  {c_type_t::VOID, "void"},
  {c_type_t::BOOL, "bool"},
  {c_type_t::CHAR, "char"},
  {c_type_t::SHORT, "short"},
  {c_type_t::INT, "int"},
  {c_type_t::LONG_INT, "long int"},
  {c_type_t::LONG_LONG_INT, "long long int"},
  {c_type_t::FLOAT, "float"},
  {c_type_t::DOUBLE, "double"},
  {c_type_t::LONG_DOUBLE, "long double"},
};

string
c_type_t::c_type_to_string() const
{
  string ret = "";
  if (this->m_is_signed) {
    ret += "signed ";
  }
  if (this->m_is_unsigned) {
    ret += "unsigned ";
  }
  ret += c_type_t::base_type_to_string(this->m_base_type) + " ";
  if (this->m_is_const) {
    ret += "const ";
  }
  return ret;
}

string
c_type_t::base_type_to_string(c_type_t::base_type_t base_type)
{
  return base_type_to_str_map.at(base_type);
}

bool
c_type_t::base_type_can_have_sign_keywords(c_type_t::base_type_t base_type)
{
  return base_type == c_type_t::CHAR ||
         base_type == c_type_t::SHORT ||
         base_type == c_type_t::INT ||
         base_type == c_type_t::LONG_INT ||
         base_type == c_type_t::LONG_LONG_INT;
}

