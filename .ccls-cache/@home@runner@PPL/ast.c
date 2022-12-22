#include "ast.h"
#include "scope.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
ast_T* init_ast(int type) {
  ast_T* ast = (ast_T*)calloc(1, sizeof(ast_T));
  ast->type = type;
  ast->children = init_list(sizeof(void*));
  ast->string_value = NULL;
  ast->value = NULL;
  ast->left = NULL;
  ast->right = NULL;
  ast->ident_value = NULL;
  ast->int_value = 0;
  ast->float_value = 0.000;
  ast->string_value = NULL;
  ast->array_children = ast->type == AST_ARRAY ? init_list(sizeof(void*)) : NULL;
  ast->function_call_args = ast->type == AST_FUNCTION_CALL ? init_list(sizeof(void*)) : NULL;
  ast->function_definition_body = NULL;
  ast->function_definition_name = NULL;
  ast->function_definition_args = ast->type==AST_FUNCTION_DEFINITION ? init_list(sizeof(ast_T*)) : NULL;
  ast->variable_name = NULL;
  ast->variable_value = NULL;
  ast->scope = init_scope();
  ast-> if_condition = NULL;
  ast-> if_body = NULL;
  ast->return_value = NULL;
  ast->index_op_value = NULL;
  ast->index = NULL;
  return ast;
}

char* ast_to_string(ast_T*ast){
  char* bufo = calloc(1024*1024, sizeof(char));
  if(ast->type == AST_ARRAY){
        strcat(bufo, "[");
    
        for(int i = 0; i < ast->array_children->used; i++) {
          if(ast->array_children->used-1 == i) {
            strcat(bufo, ast_to_string((ast_T*)ast->array_children->items[i]));
            
          } else {
            strcat(bufo, ast_to_string((ast_T*)ast->array_children->items[i]));
            strcat(bufo, ", ");
          }
        }
        strcat(bufo,"]");
    return bufo;
  }
  switch(ast->type) {
    case AST_INT:
      sprintf(bufo, "%llu", ast->int_value);
      break;
    case AST_FLOAT:
      sprintf(bufo, "%f", ast->float_value);
      break;
    case AST_STRING:
      sprintf(bufo, "\'%s\'", ast->string_value);
      break;
    case AST_BOOL:
      sprintf(bufo, "%s", ast->boolean_value? "true" : "false");
      break;
    /*case AST_BINOP: {
          ast_T* ast_data = run_ast(visitor,visited_ast);
          if(ast_data->type == AST_INT) {
            printf("%llu", ast_data->int_value);             
          }
          else if(ast_data->type == AST_FLOAT) {
            printf("%f", ast_data->float_value);             
          } else {
          printf("%s", ast_data->string_value);               
          }
        }
        case AST_NULL: printf("nil"); break;
      }*/
    case AST_FUNCTION_CALL:
      sprintf(bufo, "%s(", ast->function_call_name->ident_value);
      for(int i=0; i<ast->function_call_args->used; i++) {
        ast_T* arg = (ast_T*)ast->function_call_args->items[i];
        if(i < ast->function_call_args->used-1) {
          sprintf(bufo, "%s, ", ast_to_string(arg));
        } else {
          sprintf(bufo, "%s", ast_to_string(arg));
        }
      }
      sprintf(bufo, ")");
      break;
  } 
  if(strlen(bufo) == 0){
    sprintf(bufo, "%p", &ast);
  }
  return bufo;
}

char* astt(int type) {
  switch(type) {
    case AST_INT:
      return "int";
    case AST_FLOAT:
      return "float";
    case AST_STRING:
     return "string";
    case AST_BOOL:
     return "bool";
    case AST_ARRAY: 
     return "array";
    case AST_FUNCTION_CALL:
     return "function call";
    case AST_FUNCTION_DEFINITION:
        return "function definition";
    case AST_VARIABLE:
     return "variable";
    case AST_IDENT:
      return "ident";
    default:
    return "unknown";
    }
}