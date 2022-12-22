#include <stdio.h>
#include "io.h"
#include "lexer.h"
#include "list.h"
#include "ast.h"
#include "parser.h"
#include "visitor.h"
int main(int argc, char** argv) {
  char* file_contents = read_file_contents("main.ppl");
  Lexer_t* l = init_lexer(file_contents);
  parser_T* p = init_parser(l);
  ast_T* root = parser_parse(p, p->scope);
  visitor_T* visitor = init_visitor();
  ast_T* argn = init_ast(AST_INT);
  argn->int_value = argc;
  ast_T* argv_ast = init_ast(AST_ARRAY);

  ast_T* vardef_argn = init_ast(AST_VARIABLE_DEF);
  vardef_argn->variable_def_name = init_ast(AST_IDENT);
  vardef_argn->variable_def_name->ident_value = "argc";
  vardef_argn->variable_value = argn;
  scope_add_var(root->scope, vardef_argn);
  ast_T* vardef_argv = init_ast(AST_VARIABLE_DEF);
  vardef_argv->variable_def_name = init_ast(AST_IDENT);
  vardef_argv->variable_def_name->ident_value = "argv";
  vardef_argv->variable_value = argv_ast;
  for(int i = 0; i < argc; i++) {
    ast_T* stringelem = init_ast(AST_STRING);
    stringelem->string_value = argv[i];
    list_push(argv_ast->array_children, stringelem);
  }
  
  scope_add_var(root->scope, vardef_argv);
  visitor_visit(visitor, root);
  /*ast_T* funcmain = scope_get_func(root->scope, "Main");
  if(!funcmain) {
    fprintf(stderr, "Error: Main function not found\n");
    return 1;
  } else {
    ast_T* ast = init_ast(AST_FUNCTION_CALL);
    ast->function_call_name = init_ast(AST_IDENT);
    ast->function_call_name->ident_value="Main";
    visitor_visit_function_call(visitor, ast);
  }*/
  return 0;
}