#pragma once

#include "ast.h"

// define yyerror function for parse errors
void yyerror(translation_unit_n **root, const char *s);

