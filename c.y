%{
#include <cstdio>
#include <cassert>
#include <iostream>

#include "ast.h"
#include "common.h"

using namespace std;

// stuff from flex that bison needs to know about:
extern "C" int yylex();
int yyparse(translation_unit_n **root);
extern "C" FILE *yyin;
 
void yyerror(translation_unit_n **root, const char *s);
%}

%code requires {
  #include "ast.h"
}

%union {
  char* lex_val;
  translation_unit_n* transl_unit;
  external_declaration_n* ext_decl;
  function_definition_n* func_def;
  declaration_n* decl;
  declaration_specifiers_n* decl_specs;
  declaration_specifier_n* decl_spec;
  init_declarator_list_n* init_decl_list;
  init_declarator_n* init_decl;
  declarator_n* declarator;
  initializer_n* initializer;
  pointer_n* pointer;
  direct_declarator_n* dir_decl;
}

%parse-param {translation_unit_n **root}

%token	IDENTIFIER I_CONSTANT F_CONSTANT STRING_LITERAL FUNC_NAME SIZEOF
%token	PTR_OP INC_OP DEC_OP LEFT_OP RIGHT_OP LE_OP GE_OP EQ_OP NE_OP
%token	AND_OP OR_OP MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN ADD_ASSIGN
%token	SUB_ASSIGN LEFT_ASSIGN RIGHT_ASSIGN AND_ASSIGN
%token	XOR_ASSIGN OR_ASSIGN
%token	TYPEDEF_NAME ENUMERATION_CONSTANT

%token	TYPEDEF EXTERN STATIC AUTO REGISTER INLINE
%token	CONST RESTRICT VOLATILE
%token	BOOL CHAR SHORT INT LONG SIGNED UNSIGNED FLOAT DOUBLE VOID
%token	COMPLEX IMAGINARY 
%token	STRUCT UNION ENUM ELLIPSIS

%token	CASE DEFAULT IF ELSE SWITCH WHILE DO FOR GOTO CONTINUE BREAK RETURN

%token	ALIGNAS ALIGNOF ATOMIC GENERIC NORETURN STATIC_ASSERT THREAD_LOCAL

%type <dir_decl> direct_declarator
%type <pointer> pointer
%type <initializer> initializer
%type <declarator> declarator
%type <init_decl> init_declarator
%type <init_decl_list> init_declarator_list
%type <decl_spec> storage_class_specifier type_specifier type_qualifier function_specifier /* alignment_specifier */
%type <decl_specs> declaration_specifiers type_qualifier_list
%type <decl> declaration
%type <func_def> function_definition
%type <ext_decl> external_declaration
%type <transl_unit> translation_unit

%start translation_unit
%%

primary_expression
	: IDENTIFIER
	| constant
	| string
	| '(' expression ')'
	| generic_selection
	;

constant
	: I_CONSTANT		/* includes character_constant */
	| F_CONSTANT
	| ENUMERATION_CONSTANT	/* after it has been defined as such */
	;

//enumeration_constant		/* before it has been defined as such */
//  : IDENTIFIER
//  ;

string
	: STRING_LITERAL
	| FUNC_NAME
	;

generic_selection
	: GENERIC '(' assignment_expression ',' generic_assoc_list ')'
	;

generic_assoc_list
	: generic_association
	| generic_assoc_list ',' generic_association
	;

generic_association
	: type_name ':' assignment_expression
	| DEFAULT ':' assignment_expression
	;

postfix_expression
	: primary_expression
	| postfix_expression '[' expression ']'
	| postfix_expression '(' ')'
	| postfix_expression '(' argument_expression_list ')'
	| postfix_expression '.' IDENTIFIER
	| postfix_expression PTR_OP IDENTIFIER
	| postfix_expression INC_OP
	| postfix_expression DEC_OP
	| '(' type_name ')' '{' initializer_list '}'
	| '(' type_name ')' '{' initializer_list ',' '}'
	;

argument_expression_list
	: assignment_expression
	| argument_expression_list ',' assignment_expression
	;

unary_expression
	: postfix_expression
	| INC_OP unary_expression
	| DEC_OP unary_expression
	| unary_operator cast_expression
	| SIZEOF unary_expression
	| SIZEOF '(' type_name ')'
	| ALIGNOF '(' type_name ')'
	;

unary_operator
	: '&'
	| '*'
	| '+'
	| '-'
	| '~'
	| '!'
	;

cast_expression
	: unary_expression
	| '(' type_name ')' cast_expression
	;

multiplicative_expression
	: cast_expression
	| multiplicative_expression '*' cast_expression
	| multiplicative_expression '/' cast_expression
	| multiplicative_expression '%' cast_expression
	;

additive_expression
	: multiplicative_expression
	| additive_expression '+' multiplicative_expression
	| additive_expression '-' multiplicative_expression
	;

shift_expression
	: additive_expression
	| shift_expression LEFT_OP additive_expression
	| shift_expression RIGHT_OP additive_expression
	;

relational_expression
	: shift_expression
	| relational_expression '<' shift_expression
	| relational_expression '>' shift_expression
	| relational_expression LE_OP shift_expression
	| relational_expression GE_OP shift_expression
	;

equality_expression
	: relational_expression
	| equality_expression EQ_OP relational_expression
	| equality_expression NE_OP relational_expression
	;

and_expression
	: equality_expression
	| and_expression '&' equality_expression
	;

exclusive_or_expression
	: and_expression
	| exclusive_or_expression '^' and_expression
	;

inclusive_or_expression
	: exclusive_or_expression
	| inclusive_or_expression '|' exclusive_or_expression
	;

logical_and_expression
	: inclusive_or_expression
	| logical_and_expression AND_OP inclusive_or_expression
	;

logical_or_expression
	: logical_and_expression
	| logical_or_expression OR_OP logical_and_expression
	;

conditional_expression
	: logical_or_expression
	| logical_or_expression '?' expression ':' conditional_expression
	;

assignment_expression
	: conditional_expression
	| unary_expression assignment_operator assignment_expression
	;

assignment_operator
	: '='
	| MUL_ASSIGN
	| DIV_ASSIGN
	| MOD_ASSIGN
	| ADD_ASSIGN
	| SUB_ASSIGN
	| LEFT_ASSIGN
	| RIGHT_ASSIGN
	| AND_ASSIGN
	| XOR_ASSIGN
	| OR_ASSIGN
	;

expression
	: assignment_expression
	| expression ',' assignment_expression
	;

constant_expression
	: conditional_expression	/* with constraints */
	;

declaration
	: declaration_specifiers ';' { $$ = new declaration_n($1); }
	| declaration_specifiers init_declarator_list ';' { $$ = new declaration_n($1, $2); }
	// | static_assert_declaration
	;

declaration_specifiers
	: storage_class_specifier declaration_specifiers {
	  $$ = $2;
	  $$->add_child_front($1);
	}
	| storage_class_specifier {
	  $$ = new declaration_specifiers_n();
	  $$->add_child($1);
	}
	| type_specifier declaration_specifiers {
	  $$ = $2;
	  $$->add_child_front($1);
	}
	| type_specifier {
	  $$ = new declaration_specifiers_n();
	  $$->add_child($1);
	}
	| type_qualifier declaration_specifiers {
	  $$ = $2;
	  $$->add_child_front($1);
	}
	| type_qualifier {
    $$ = new declaration_specifiers_n();
    $$->add_child($1);
  }
	| function_specifier declaration_specifiers {
	  $$ = $2;
	  $$->add_child_front($1);
	}
	| function_specifier {
	  $$ = new declaration_specifiers_n();
	  $$->add_child($1);
	}
//	| alignment_specifier declaration_specifiers
//	| alignment_specifier
	;

init_declarator_list
	: init_declarator {
	  $$ = new init_declarator_list_n();
	  $$->add_child($1);
	}
	| init_declarator_list ',' init_declarator {
	  $$ = $1;
	  $$->add_child($3);
	}
	;

init_declarator
//	: declarator '=' initializer { $$ = new init_declarator_n($1, $3); }
	: declarator { $$ = new init_declarator_n($1); }
	;

storage_class_specifier
	: TYPEDEF { $$ = new declaration_specifier_n(specifier::TYPEDEF); }	/* identifiers must be flagged as TYPEDEF_NAME */
	| EXTERN { $$ = new declaration_specifier_n(specifier::EXTERN); }
	| STATIC { $$ = new declaration_specifier_n(specifier::STATIC); }
	| THREAD_LOCAL { $$ = new declaration_specifier_n(specifier::THREAD_LOCAL); }
	| AUTO { $$ = new declaration_specifier_n(specifier::AUTO); }
	| REGISTER { $$ = new declaration_specifier_n(specifier::RESTRICT); }
	;

type_specifier
	: VOID { $$ = new declaration_specifier_n(specifier::VOID); }
	| CHAR { $$ = new declaration_specifier_n(specifier::CHAR); }
	| SHORT { $$ = new declaration_specifier_n(specifier::SHORT); }
	| INT { $$ = new declaration_specifier_n(specifier::INT); }
	| LONG { $$ = new declaration_specifier_n(specifier::LONG); }
	| FLOAT { $$ = new declaration_specifier_n(specifier::FLOAT); }
	| DOUBLE { $$ = new declaration_specifier_n(specifier::DOUBLE); }
	| SIGNED { $$ = new declaration_specifier_n(specifier::SIGNED); }
	| UNSIGNED { $$ = new declaration_specifier_n(specifier::UNSIGNED); }
	| BOOL { $$ = new declaration_specifier_n(specifier::BOOL); }
	| COMPLEX { $$ = new declaration_specifier_n(specifier::COMPLEX); }
	| IMAGINARY { $$ = new declaration_specifier_n(specifier::IMAGINARY); }	  	/* non-mandated extension */
	// | atomic_type_specifier
	// | struct_or_union_specifier
	// | enum_specifier
	// | TYPEDEF_NAME		/* after it has been defined as such */
	;

//struct_or_union_specifier
//	: struct_or_union '{' struct_declaration_list '}'
//	| struct_or_union IDENTIFIER '{' struct_declaration_list '}'
//	| struct_or_union IDENTIFIER
//	;

//struct_or_union
//	: STRUCT
//	| UNION
//	;

//struct_declaration_list
//	: struct_declaration
//	| struct_declaration_list struct_declaration
//	;

//struct_declaration
//	: specifier_qualifier_list ';'	/* for anonymous struct/union */
//	| specifier_qualifier_list struct_declarator_list ';'
//	| static_assert_declaration
//	;

specifier_qualifier_list
	: type_specifier specifier_qualifier_list
	| type_specifier
	| type_qualifier specifier_qualifier_list
	| type_qualifier
	;

//struct_declarator_list
//	: struct_declarator
//	| struct_declarator_list ',' struct_declarator
//	;

//struct_declarator
//	: ':' constant_expression
//	| declarator ':' constant_expression
//	| declarator
//	;

//enum_specifier
//	: ENUM '{' enumerator_list '}'
//	| ENUM '{' enumerator_list ',' '}'
//	| ENUM IDENTIFIER '{' enumerator_list '}'
//	| ENUM IDENTIFIER '{' enumerator_list ',' '}'
//	| ENUM IDENTIFIER
//	;

//enumerator_list
//	: enumerator
//	| enumerator_list ',' enumerator
//	;

//enumerator	/* identifiers must be flagged as ENUMERATION_CONSTANT */
//	: enumeration_constant '=' constant_expression
//	| enumeration_constant
//	;

//atomic_type_specifier
//	: ATOMIC '(' type_name ')'
//	;

type_qualifier
	: CONST { $$ = new declaration_specifier_n(specifier::CONST); }
	| RESTRICT { $$ = new declaration_specifier_n(specifier::RESTRICT); }
	| VOLATILE { $$ = new declaration_specifier_n(specifier::VOLATILE); }
	| ATOMIC { $$ = new declaration_specifier_n(specifier::ATOMIC); }
	;

function_specifier
	: INLINE { $$ = new declaration_specifier_n(specifier::INLINE); }
	| NORETURN { $$ = new declaration_specifier_n(specifier::NORETURN); }
	;

//alignment_specifier
//	: ALIGNAS '(' type_name ')'
//	| ALIGNAS '(' constant_expression ')'
//	;

declarator
	: pointer direct_declarator { $$ = new declarator_n($1, $2); }
	| direct_declarator { $$ = new declarator_n($1); }
	;

direct_declarator
	: IDENTIFIER { $$ = new direct_declarator_n(); }
//	| '(' declarator ')'
//	| direct_declarator '[' ']'
//	| direct_declarator '[' '*' ']'
//	| direct_declarator '[' STATIC type_qualifier_list assignment_expression ']'
//	| direct_declarator '[' STATIC assignment_expression ']'
//	| direct_declarator '[' type_qualifier_list '*' ']'
//	| direct_declarator '[' type_qualifier_list STATIC assignment_expression ']'
//	| direct_declarator '[' type_qualifier_list assignment_expression ']'
//	| direct_declarator '[' type_qualifier_list ']'
//	| direct_declarator '[' assignment_expression ']'
	| direct_declarator '(' parameter_type_list ')' {
	  $$ = $1;
	}
	| direct_declarator '(' ')' {
	  $$ = $1;
  }
	| direct_declarator '(' identifier_list ')' {
    $$ = $1;
  }
	;

pointer
	: '*' type_qualifier_list pointer {
	  $$ = $3;
	  $$->add_child_front($2);
	}
	| '*' type_qualifier_list {
	  $$ = new pointer_n();
	  $$->add_child_front($2);
	}
	| '*' pointer {
	  $$ = $2;
	  $$->add_child_front(nullptr);
	}
	| '*' {
	  $$ = new pointer_n();
	  $$->add_child_front(nullptr);
	}
	;

type_qualifier_list
	: type_qualifier {
	  assert($1->is_type_qualifier());
	  $$ = new declaration_specifiers_n();
	  $$->add_child($1);
	}
	| type_qualifier_list type_qualifier {
	  assert($2->is_type_qualifier());
	  $$ = $1;
	  $$->add_child($2);
	}
	;


parameter_type_list
	: parameter_list ',' ELLIPSIS
	| parameter_list
	;

parameter_list
	: parameter_declaration
	| parameter_list ',' parameter_declaration
	;

parameter_declaration
	: declaration_specifiers declarator
	| declaration_specifiers abstract_declarator
	| declaration_specifiers
	;

identifier_list
	: IDENTIFIER
	| identifier_list ',' IDENTIFIER
	;

type_name
	: specifier_qualifier_list abstract_declarator
	| specifier_qualifier_list
	;

abstract_declarator
	: pointer direct_abstract_declarator
	| pointer
	| direct_abstract_declarator
	;

direct_abstract_declarator
	: '(' abstract_declarator ')'
	| '[' ']'
	| '[' '*' ']'
	| '[' STATIC type_qualifier_list assignment_expression ']'
	| '[' STATIC assignment_expression ']'
	| '[' type_qualifier_list STATIC assignment_expression ']'
	| '[' type_qualifier_list assignment_expression ']'
	| '[' type_qualifier_list ']'
	| '[' assignment_expression ']'
	| direct_abstract_declarator '[' ']'
	| direct_abstract_declarator '[' '*' ']'
	| direct_abstract_declarator '[' STATIC type_qualifier_list assignment_expression ']'
	| direct_abstract_declarator '[' STATIC assignment_expression ']'
	| direct_abstract_declarator '[' type_qualifier_list assignment_expression ']'
	| direct_abstract_declarator '[' type_qualifier_list STATIC assignment_expression ']'
	| direct_abstract_declarator '[' type_qualifier_list ']'
	| direct_abstract_declarator '[' assignment_expression ']'
	| '(' ')'
	| '(' parameter_type_list ')'
	| direct_abstract_declarator '(' ')'
	| direct_abstract_declarator '(' parameter_type_list ')'
	;

initializer
//	: '{' initializer_list '}'
//	| '{' initializer_list ',' '}'
	: assignment_expression { $$ = new initializer_n(); }
	;

initializer_list
	: designation initializer
	| initializer
	| initializer_list ',' designation initializer
	| initializer_list ',' initializer
	;

designation
	: designator_list '='
	;

designator_list
	: designator
	| designator_list designator
	;

designator
	: '[' constant_expression ']'
	| '.' IDENTIFIER
	;

//static_assert_declaration
//	: STATIC_ASSERT '(' constant_expression ',' STRING_LITERAL ')' ';'
//	;

statement
	: labeled_statement
	| compound_statement
	| expression_statement
	| selection_statement
	| iteration_statement
	| jump_statement
	;

labeled_statement
	: IDENTIFIER ':' statement
	| CASE constant_expression ':' statement
	| DEFAULT ':' statement
	;

compound_statement
	: '{' '}'
	| '{'  block_item_list '}'
	;

block_item_list
	: block_item
	| block_item_list block_item
	;

block_item
	: declaration
	| statement
	;

expression_statement
	: ';'
	| expression ';'
	;

selection_statement
	: IF '(' expression ')' statement ELSE statement
	| IF '(' expression ')' statement
	| SWITCH '(' expression ')' statement
	;

iteration_statement
	: WHILE '(' expression ')' statement
	| DO statement WHILE '(' expression ')' ';'
	| FOR '(' expression_statement expression_statement ')' statement
	| FOR '(' expression_statement expression_statement expression ')' statement
	| FOR '(' declaration expression_statement ')' statement
	| FOR '(' declaration expression_statement expression ')' statement
	;

jump_statement
	: GOTO IDENTIFIER ';'
	| CONTINUE ';'
	| BREAK ';'
	| RETURN ';'
	| RETURN expression ';'
	;

translation_unit
	: external_declaration {
	  *root = new translation_unit_n();
	  $$ = *root;
	  $$->add_child($1);
	}
	| translation_unit external_declaration { $$ = $1; $$->add_child($2); }
	;

external_declaration
	: function_definition { $$ = new external_declaration_n($1); }
	| declaration { $$ = new external_declaration_n($1); }
	;

function_definition
	: declaration_specifiers declarator declaration_list compound_statement {
    $$ = new function_definition_n($1);
  }
	| declaration_specifiers declarator compound_statement {
    $$ = new function_definition_n($1);
  }
	;

declaration_list
	: declaration
	| declaration_list declaration
	;

%%
#include <stdio.h>

void yyerror(translation_unit_n **root, const char *s)
{
	fflush(stdout);
	fprintf(stderr, "*** %s\n", s);
}
