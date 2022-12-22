#ifndef PPL_SCOPE_H
#define PPL_SCOPE_H
#include "list.h"
#include "ast.h"
typedef struct SCOPE_STRUCT
{
List* funcs;
List* vars;
} scope_T;

scope_T* init_scope();
ast_T* scope_add_func(scope_T* scope, ast_T* fdef);
ast_T* scope_get_func(scope_T* scope, const char* fname);
ast_T* scope_add_var(scope_T* scope, ast_T* vdef);
ast_T* scope_get_var(scope_T* scope, const char* vname);
ast_T* scope_remove_var(scope_T* scope, const char* vname);
int scope_get_varindex(scope_T* scope, const char* vname);
#endif