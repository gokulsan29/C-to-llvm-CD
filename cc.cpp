#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <iostream>

#include "ast.h"
#include "c.tab.hpp"

extern "C" int yylex();
int yyparse(translation_unit_n **root);
extern "C" FILE *yyin;

static void usage()
{
  printf("Usage: cc <prog.c>\n");
}

int
main(int argc, char **argv)
{
  if (argc != 2) {
    usage();
    exit(1);
  }
  char const *filename = argv[1];
  yyin = fopen(filename, "r");
  assert(yyin);

  translation_unit_n *root = nullptr;
  int ret = yyparse(&root);
  std::cout << root->to_string() << "\n";
  printf("retv = %d\n", ret);
  exit(0);
}
