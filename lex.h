#pragma once

#include <stdio.h>

// stuff from flex that bison needs to know about:
extern "C" int yylex();
extern "C" FILE *yyin;

