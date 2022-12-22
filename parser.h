#ifndef PPL_PARSER_H
#define PPL_PARSER_H
#include "lexer.h"
#include "list.h"
#include "ast.h"
#include "scope.h"
typedef struct PARSER_STRUCT {
  Lexer_t* l;
  Token_t* curToken;
  Token_t* peekToken;

  List* errors;
  scope_T* scope;
} parser_T;

parser_T* init_parser(Lexer_t* l);
void add_error(parser_T* p, char* msg);
void peek_error(parser_T* p, enum TokenType typ, Token_t* kind);
void parser_eat(parser_T* p, enum TokenType typ);
void next_token(parser_T* p);
int curTokenIs(parser_T* p, enum TokenType kind);
int peekTokenIs(parser_T* p, enum TokenType kind);
ast_T* parser_parse(parser_T* p, scope_T* scope);
ast_T* parse_ident(parser_T* p, scope_T* scope);
ast_T* parse_int(parser_T* p, scope_T* scope);
ast_T* parse_float(parser_T* p, scope_T* scope);
ast_T* parse_string(parser_T* p, scope_T* scope);
ast_T* parse_bool(parser_T* p, scope_T* scope);
ast_T* parse_expr(parser_T* p, scope_T* scope);
ast_T* parse_var_declaration(parser_T* p, scope_T* scope);
ast_T* parse_func_call(parser_T* p, scope_T* scope);
ast_T* parse_func_def(parser_T* p, scope_T* scope);
ast_T* parse_array(parser_T* p, scope_T* scope);
ast_T* parse_variable(parser_T* p, scope_T* scope);
ast_T* parse_nil(parser_T* p, scope_T* scope);
ast_T* parse_binop(parser_T* p, scope_T* scope);
ast_T* parse_term(parser_T* p, scope_T* scope);
ast_T* parse_return(parser_T* p, scope_T* scope);
ast_T* parse_break(parser_T* p, scope_T* scope);
ast_T* parse_continue(parser_T* p, scope_T* scope);
ast_T* parse_statement(parser_T* p, scope_T* scope);
ast_T* parse_statements(parser_T* p, scope_T* scope);
ast_T* parse_boolean_expr(parser_T* p, scope_T* scope);
ast_T* parse_if_statement(parser_T*, scope_T*);
ast_T* parse_for_statement(parser_T* p, scope_T* scope);
ast_T* parse_struct_statement(parser_T* p, scope_T* scope);
ast_T* parse_enum_statement(parser_T* p, scope_T* scope);
ast_T* parse_while_statement(parser_T* p, scope_T* scope);
ast_T* parse_variable_reassignment(parser_T* p, scope_T* scope);
ast_T* parse_index_op(parser_T* p, scope_T* scope);
#endif