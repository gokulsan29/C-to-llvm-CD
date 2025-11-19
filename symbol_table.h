#pragma once

#include <map>

#include "c_type.h"

class symbol_table_t
{
public:
  symbol_table_t() : m_prev_scope(nullptr) { }
  symbol_table_t(symbol_table_t* prev_scope) : m_prev_scope(prev_scope) { }
private:
  map<string, c_type_t*> m_table;
  symbol_table_t* m_prev_scope;
};

