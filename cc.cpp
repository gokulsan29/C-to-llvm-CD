#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "c.tab.hpp"
#include "lex.h"

static void usage()
{
  printf("Usage: cc <prog.c>\n");
}

int
main(int argc, char **argv)
{
  if (argc < 2) {
    usage();
    exit(1);
  }
  char const *filename = argv[1];
  yyin = fopen(filename, "r");
  assert(yyin);

  bool show_ast = false;
  for (int i = 2;i < argc;i++) {
    if (strcmp(argv[i], "--show-ast") == 0) {
      show_ast = true;
    }
    else {
      std::cout << "Invalid arg: " << argv[i] << std::endl;
      exit(1);
    }
  }

  translation_unit_n *root = new translation_unit_n(filename);
  int ret = yyparse(&root);
  if (show_ast) {
    std::cout << root->to_string_ast() << "\n\n";
  }
  printf("retv = %d\n", ret);
  root->llvm_codegen();
  exit(0);
}
