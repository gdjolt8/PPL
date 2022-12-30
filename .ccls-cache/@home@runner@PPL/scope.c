#include "scope.h"

scope_T* init_scope() {
  scope_T* scope = calloc(1, sizeof(scope_T));
  scope->funcs = init_list(sizeof(struct AST_STRUCT*));
  scope->vars = init_list(sizeof(struct AST_STRUCT*));
  return scope;
}
ast_T* scope_add_func(scope_T* scope, ast_T* fdef) {
  list_push(scope->funcs, fdef);
  return fdef;
}
ast_T* scope_get_func(scope_T* scope, const char* fname) {
  for(int i = 0; i < scope->funcs->used; i++) {
    ast_T* fdef = (ast_T*)scope->funcs->items[i];
    if(strcmp(fdef->function_definition_name->ident_value, fname) == 0) {
      return fdef;
    }
    
  }
  return NULL;
}
ast_T* scope_add_var(scope_T* scope, ast_T* vdef) {
  
  for(int i = 0; i < scope->vars->used; i++) {
    ast_T* def = (ast_T*)scope->vars->items[i];
    if(strcmp(def->variable_def_name->ident_value, vdef->variable_def_name->ident_value) == 0) {
      scope_remove_var(scope, def->variable_def_name->ident_value);
    }
  }
  list_push(scope->vars, vdef);
  return vdef;
}
ast_T* scope_get_var(scope_T* scope, const char* vname) {
  for(int i = 0; i < scope->vars->used; i++) {
    ast_T* vdef = (ast_T*)scope->vars->items[i];
    //printf("Scope vn:%s, vv:%d\n\n", vdef->variable_def_name->ident_value, vdef->variable_value->type);
    if(strcmp(vdef->variable_def_name->ident_value, vname) == 0) {
      return vdef;
    }
  }
  return NULL;
}

ast_T* scope_remove_var(scope_T* scope, const char* vname) {
     for(int i = 0; i < scope->vars->used; i++) {
    ast_T* fdef = (ast_T*)scope->vars->items[i];
    if(strcmp(fdef->variable_def_name->ident_value, vname) == 0) {
      ast_T* dm = init_ast(AST_VARIABLE_DEF);
      dm->variable_def_name = init_ast(AST_IDENT);
      dm->variable_def_name->ident_value = "";
      scope->vars->items[i] = dm;
    }
  }
  return NULL;
}

int scope_get_varindex(scope_T* scope, const char* vname) {
  for(int i = 0; i < scope->vars->used; i++) {
    ast_T* fdef = (ast_T*)scope->vars->items[i];
    if(strcmp(fdef->variable_def_name->ident_value, vname) == 0) {
      return i;
    }
  }
  return -1;
}