#ifndef PPL_VISITOR_H
#define PPL_VISITOR_H
#include "ast.h"
typedef struct VISITOR_STRUCT {} visitor_T;
visitor_T* init_visitor();
ast_T* visitor_visit(visitor_T* visitor, ast_T* node);
ast_T* visitor_visit_variable_def(visitor_T* visitor, ast_T* node);
ast_T* visitor_visit_function_def(visitor_T* visitor, ast_T* node);
ast_T* visitor_visit_variable(visitor_T* visitor, ast_T* node);
ast_T* visitor_visit_function_call(visitor_T* visitor, ast_T* node);
ast_T* visitor_visit_return(visitor_T* visitor, ast_T* node);
ast_T* visitor_visit_binop(visitor_T* visitor, ast_T* node);
ast_T* visitor_visit_ident(visitor_T* visitor, ast_T* node);
ast_T* visitor_visit_int(visitor_T* visitor, ast_T* node);
ast_T* visitor_visit_float(visitor_T* visitor, ast_T* node);
ast_T* visitor_visit_string(visitor_T* visitor, ast_T* node);
ast_T* visitor_visit_bool(visitor_T* visitor, ast_T* node);
ast_T* visitor_visit_compound(visitor_T* visitor, ast_T* node);
ast_T* visitor_visit_if(visitor_T* visitor, ast_T* node);
ast_T* visitor_visit_for(visitor_T* visitor, ast_T* node);
ast_T* visitor_visit_while(visitor_T* visitor, ast_T* node);
ast_T* visitor_visit_break(visitor_T* visitor, ast_T* node);
ast_T* visitor_visit_continue(visitor_T* visitor, ast_T* node);
ast_T* visitor_visit_indexop(visitor_T* visitor, ast_T* node);
ast_T* visitor_visit_array(visitor_T* visitor, ast_T* node);
ast_T* visitor_visit_variable_reassignment(visitor_T* visitor, ast_T* node);
ast_T* visitor_visit_boolean_expr(visitor_T* visitor, ast_T* node);
ast_T* run_boolexpr(visitor_T* visitor, ast_T* node);
void visitor_expect_args(visitor_T* visitor, List* args, int argc, int arguments[]);
#endif