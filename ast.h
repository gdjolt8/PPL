#ifndef PPL_AST_H
#define PPL_AST_H
#include "list.h"
#include "token.h"
typedef struct AST_STRUCT {
  enum {
    AST_COMPOUND,
    AST_VARIABLE,
    AST_VARIABLE_DEF,
    AST_STMT_RETURN,
    AST_ACCESS,
    AST_ASSIGNMENT,
    AST_FUNCTION_CALL,
    AST_FUNCTION_DEFINITION,
    AST_INT,
    AST_IDENT,
    AST_FLOAT,
    AST_STRING,
    AST_BOOL,
    AST_OBJECT,
    AST_BINOP,
    AST_UNOP,
    AST_NOOP,
    AST_ARRAY,
    AST_ANY,
    AST_IF,
    AST_WHILE,
    AST_BREAK,
    AST_CONTINUE,
    AST_VARIABLE_REASSIGNMENT,
    AST_INDEXOP,
    AST_FOR,
    AST_STRT,
    AST_MATCH,
    AST_IMPORT,
    AST_NULL
  } type;
  List* children;

  unsigned int variable_const;
  struct AST_STRUCT* variable_def_name;
  struct AST_STRUCT* variable_value;

  struct AST_STRUCT* function_definition_body;
  struct AST_STRUCT* function_definition_name;
  struct AST_STRUCT* function_definition_return_value;
  List* function_definition_args;

  struct AST_STRUCT* function_call_name;
  List* function_call_args;
  struct AST_STRUCT* value;
  struct AST_STRUCT* left;
  struct AST_STRUCT* right;
  enum TokenType op;
  char* ident_value;
  char* string_value;
  long long int_value;
  long double float_value;

  struct AST_STRUCT* boolean_left;
  enum TokenType bop;
  struct AST_STRUCT* boolean_right;
  unsigned int boolean_value;
  unsigned int boolean_m1;
  List* array_children;
  struct SCOPE_STRUCT* scope;
  struct AST_STRUCT* variable_name;

  // if statement
  struct AST_STRUCT* if_condition;
  struct AST_STRUCT* if_body;

  struct AST_STRUCT* struct_name;
  struct AST_STRUCT* struct_body;

  struct AST_STRUCT* enum_name;
  struct AST_STRUCT* enum_body;

  List* object_keys;
  List* object_values;

  struct AST_STRUCT* match_body;

  struct AST_STRUCT* import_file;

  struct AST_STRUCT* for_var_test;
  struct AST_STRUCT* for_iterator;
  struct AST_STRUCT* for_body;

  struct AST_STRUCT* while_condition;
  struct AST_STRUCT* while_body;

  enum TokenType vop;
  struct AST_STRUCT* variable_reassignment_value;

  struct AST_STRUCT* index_op_value;
  struct AST_STRUCT* index;
  struct AST_STRUCT* index_left;

  struct AST_STRUCT* access_left;
  struct AST_STRUCT* access_right;
  struct AST_STRUCT* access_value;

  struct AST_STRUCT* return_value;
} ast_T;

ast_T* init_ast(int type);
char* ast_to_string(ast_T*ast);
char* astt(int type);
#endif