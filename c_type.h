#pragma once

#include <assert.h>
#include <string>

using namespace std;

class c_type_t
{
public:
  enum base_type_t
  {
    NO_TYPE = 0,
    VOID,
    BOOL,
    CHAR,
    SHORT,
    INT,
    LONG_INT,
    LONG_LONG_INT,
    FLOAT,
    DOUBLE,
    LONG_DOUBLE,
  };

  c_type_t(base_type_t base_type,
           bool is_const,
           bool is_signed,
           bool is_unsigned) :
    m_base_type(base_type),
    m_is_const(is_const),
    m_is_signed(is_signed),
    m_is_unsigned(is_unsigned)
  {
    assert(!is_signed || !is_unsigned);
    assert(!is_const || base_type_can_have_sign_keywords(base_type));
  }

  string c_type_to_string() const;

  static string base_type_to_string(base_type_t base_type);
  static bool base_type_can_have_sign_keywords(base_type_t base_type);
private:
  base_type_t m_base_type = NO_TYPE;
  bool m_is_const = false;
  bool m_is_signed = false;
  bool m_is_unsigned = false;
};

