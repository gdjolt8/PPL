#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

parser_T* init_parser(Lexer_t* l) {
  parser_T* p = calloc(1, sizeof(parser_T));
  p->l = l;
  p->curToken = p->peekToken = NULL;
  p->errors = init_list(sizeof(char*));
  p->scope = init_scope();
  next_token(p);
  next_token(p);
  return p;
}

unsigned int is_data_type(parser_T* p) {
  return (p->curToken->kind >= KW_Int && p->curToken->kind <= KW_String);
}

unsigned int peek_is_atype(parser_T* p) {
  return (peekTokenIs(p, PlusEq) || peekTokenIs(p, MinusEq) || peekTokenIs(p, AsteriskEq) || peekTokenIs(p, SlashEq) || peekTokenIs(p, Eq));
}


void add_error(parser_T* p, char* msg) {
  char* Msg = calloc(strlen(msg) + 50, sizeof(char));
  sprintf(Msg, "[Line %d, Column %d] %s", p->l->line, p->l->column, msg);
  list_push(p->errors, Msg);
  printf("%s\n", p->errors[p->errors->used - 1]);
  free(Msg);
  exit(1);
}

void peek_error(parser_T* p, enum TokenType typ, Token_t* kind) {
  /*if(!(typ==kind->kind)){
    char* msg = calloc(1000, sizeof(char));
    sprintf(msg2, "Expected %s got '%s'", typeToStr(typ), kind->text);
    add_error(p, msg2);
  }*/
}

void parser_eat(parser_T* p, enum TokenType typ) {
  if(p->curToken->kind==typ) {
    next_token(p);
  } else {
    printf("Expected '%s' got '%s'\n", typeToStr(typ), p->curToken->text);
    exit(1);
  }
}

void parser_eat2(parser_T* p, enum TokenType typ) {
  if(p->peekToken->kind==typ) {
    next_token(p);
  } else {
    peek_error(p, typ, p->peekToken);
    printf("Expected '%s' got '%s'\n", typeToStr(typ), p->peekToken->text);
    exit(1);
  }
}
void next_token(parser_T* p) {
  p->curToken = p->peekToken;
  p->peekToken = next_token_lexer(p->l);
}
int curTokenIs(parser_T* p, enum TokenType kind){
  return p->curToken->kind == kind;
}
int peekTokenIs(parser_T* p, enum TokenType kind) {
  return p->peekToken->kind==kind;
}

ast_T* parser_parse(parser_T* p, scope_T* scope) {
  return parse_statements(p, scope);
}

ast_T* parse_statement(parser_T* p, scope_T* scope) {
  switch (p->curToken->kind)
  {
      case KW_Var: return parse_var_declaration(p, scope);
      case KW_Func: return parse_func_def(p, scope);
      case Ident: 
        if(peek_is_atype(p)) {
          printf("Is atype\n");
          return parse_variable_reassignment(p, scope);
        }
        return parse_variable(p, scope);
      case KW_Return: return parse_return(p, scope);
      case KW_If: return parse_if_statement(p, scope);
      case KW_For: return parse_for_statement(p, scope);
      case KW_While: return parse_while_statement(p, scope);
      case KW_Break: return parse_break(p, scope);
      case KW_Continue: return parse_continue(p, scope);
      case KW_Match: return parse_match(p, scope);
      case KW_Import: return parse_import(p, scope);
  }
  return init_ast(AST_NULL);

}

ast_T* parse_statementss(parser_T* p, scope_T* scope) {
  ast_T* compound = init_ast(AST_COMPOUND);
  compound->scope = scope;
  ast_T* ast_statement = parse_statement(p, scope);
  ast_statement->scope = scope;
  
  list_push(compound->children, ast_statement);
  while(!curTokenIs(p, Rbrace)) {
    if(curTokenIs(p, Rparen)) parser_eat(p, Rparen);
    if(curTokenIs(p, Semicolon)) parser_eat(p, Semicolon);
    ast_statement = parse_statement(p, scope);
    ast_statement->scope = scope;
    if(ast_statement) {
        list_push(compound->children, ast_statement);
    }
  }
  return compound;
}
ast_T* parse_statements(parser_T* p, scope_T* scope) {
  ast_T* compound = init_ast(AST_COMPOUND);
  compound->scope = scope;
  ast_T* ast_statement = parse_statement(p, scope);
  list_push(compound->children, ast_statement);
  while(!curTokenIs(p, END_OF_FILE)) {
    ast_statement = parse_statement(p, scope);
    list_push(compound->children, ast_statement);
    if(curTokenIs(p, Rbrace)) parser_eat(p, Rbrace);
    if(curTokenIs(p, Rparen)) parser_eat(p, Rparen);
    if(curTokenIs(p, Semicolon)) parser_eat(p, Semicolon);
  }
  return compound;
}
ast_T* parse_ident(parser_T* p, scope_T* scope) {
  ast_T* ast = init_ast(AST_IDENT);
  ast->ident_value = p->curToken->text;
  ast->scope = scope;
  return ast;
}

ast_T* parse_int(parser_T* p, scope_T* scope) {
  ast_T* ast = init_ast(AST_INT);
  ast->int_value = atoi(p->curToken->text);
  ast->float_value = (float)ast->int_value;
  ast->scope = scope;
  return ast;
}
ast_T* parse_float(parser_T* p, scope_T* scope) {
  ast_T* ast = init_ast(AST_FLOAT);
  ast->float_value = atof(p->curToken->text);
  ast->int_value = (int)ast->float_value;
  ast->scope = scope;
  return ast;
}
ast_T* parse_string(parser_T* p, scope_T* scope) {
  ast_T* ast = init_ast(AST_STRING);
  ast->string_value = (char*)calloc(strlen(p->curToken->text)+1, sizeof(char));
  strcpy(ast->string_value, p->curToken->text);
  ast->scope = scope;
  return ast;
}
ast_T* parse_bool(parser_T* p, scope_T* scope) {
  ast_T* ast = init_ast(AST_BOOL);
  ast->boolean_value = strcmp(p->curToken->text, "true") == 0 ? 1 : 0;
  ast->scope = scope;
  return ast;
}

ast_T* parse_expr(parser_T* p, scope_T* scope) {
  switch(p->curToken->kind) {
    case Ident:
      if(peek_is_atype(p)) {
        return parse_variable_reassignment(p, scope);
      }
      return parse_variable(p,scope);
    case Int:
      return parse_int(p, scope);
    case Float:
      return parse_float(p, scope);
    case String:
      return parse_string(p, scope);
    case Lbrack:
      return parse_array(p, scope);
    case KW_True:
    case KW_False:
      return parse_boolean_expr(p, scope);
    case KW_Var:
      return parse_var_declaration(p, scope);
    case KW_Func:
      return parse_func_def(p, scope);
    default: {
      fprintf(stderr, "Unexpected expression: '%s'\n", p->curToken->text);
      exit(1);
      return init_ast(AST_NULL);
    }
  }
}

ast_T* parse_var_declaration(parser_T* p, scope_T* scope) {
  ast_T* ast = init_ast(AST_VARIABLE_DEF);
  parser_eat(p, KW_Var); // iden
  if(curTokenIs(p, KW_Const)) {
    ast->variable_const = 1;
    parser_eat(p, KW_Const);
  } else {
    ast->variable_const = 0;
  }
  ast->variable_def_name = parse_ident(p, scope);
  parser_eat(p, Ident); // eq
  if(curTokenIs(p, Semicolon)) {
    ast->variable_value = init_ast(AST_NULL);
    ast->scope = scope;
    return ast;
  }
  parser_eat(p, Eq); // expression
  ast->variable_value = parse_binop(p, scope);
  ast->scope = scope;
  return ast;
}

ast_T* parse_func_call(parser_T* p, scope_T* scope) {
  ast_T* ast = init_ast(AST_FUNCTION_CALL);
  ast->function_call_name = parse_ident(p, scope);
  ast->function_call_args=init_list(sizeof(ast_T*));
  ast->scope = scope;
  parser_eat(p, Ident);
  parser_eat(p, Lparen); // expr or rparen
  if(!curTokenIs(p, Rparen)) {
    ast_T* expr = parse_binop(p, scope);
    list_push(ast->function_call_args, expr);  
  }
  while(curTokenIs(p, Comma)) {
    parser_eat(p, Comma);
    list_push(ast->function_call_args, parse_binop(p, scope));    
  }
  
  return ast;
}
ast_T* parse_func_def(parser_T* p, scope_T* scope) {
  ast_T* ast = init_ast(AST_FUNCTION_DEFINITION);
  parser_eat(p, KW_Func);
  ast->function_definition_name = parse_ident(p, scope);
  parser_eat(p, Ident);
  parser_eat(p, Lparen);

  if(!curTokenIs(p, Rparen)) {
    ast_T* expr = parse_variable(p, scope);
    parser_eat(p, Ident);
    list_push(ast->function_definition_args, expr);
  } 
  
  while(curTokenIs(p, Comma)) {
    parser_eat(p, Comma);
    ast_T* arg = parse_variable(p, scope);
    list_push(ast->function_definition_args, arg);    
    parser_eat(p, Ident);
  }
  parser_eat(p, Rparen);
  parser_eat(p, Lbrace);
  ast_T* compound = init_ast(AST_COMPOUND);
  ast->function_definition_return_value = init_ast(AST_NULL);
  ast_T* ast_statement = parse_statement(p, scope);
  ast_statement->scope = scope;
  int return_amounts = 0;
  
  list_push(compound->children, ast_statement);
  if(ast_statement->type == AST_STMT_RETURN) {
      
      if(return_amounts < 1)
        ast->function_definition_return_value = ast_statement->return_value;
      return_amounts++;
    }
  while(!curTokenIs(p, Rbrace)) {
    if(curTokenIs(p, Rparen)) parser_eat(p, Rparen);
    if(curTokenIs(p, Semicolon)) parser_eat(p, Semicolon);
    ast_statement = parse_statement(p, scope);
  ast_statement->scope = scope;

    if(ast_statement->type == AST_STMT_RETURN) {
      
      if(return_amounts < 1)
        ast->function_definition_return_value = ast_statement->return_value;
        return_amounts++;
    }
    if(ast_statement && return_amounts < 1) {
        list_push(compound->children, ast_statement);
    }
  }
  ast->function_definition_body = compound;
  ast->function_definition_body->scope = scope;
  ast->scope = scope;
  return ast;
}
ast_T* parse_array(parser_T* p, scope_T* scope) {
  parser_eat(p,Lbrack);
  ast_T* ast = init_ast(AST_ARRAY);
  if(!curTokenIs(p, Rbrack)) {
    ast_T* expr = parse_binop(p, scope);
    list_push(ast->array_children, expr);
  }
  while(curTokenIs(p, Comma)) {
    parser_eat(p, Comma);
    list_push(ast->array_children, parse_binop(p, scope));
  }
  ast->scope = scope;
  return ast;
}

ast_T* parse_variable(parser_T* p, scope_T* scope) {
  ast_T* ast_variable = init_ast(AST_VARIABLE);
  ast_variable->variable_name = parse_ident(p, scope);
  ast_variable->scope = scope;
  if(peekTokenIs(p, Lparen)) {
    return parse_func_call(p, scope);
  }
  if(!peekTokenIs(p, Dot)) {
    return ast_variable;
  }
  next_token(p);
  while(curTokenIs(p, Dot)) {
    ast_T* ast_accop = init_ast(AST_ACCESS);
    ast_accop->access_left = ast_variable;
    parser_eat(p, Dot);
    ast_accop->access_right = parse_expr(p, scope);
    ast_accop->scope = scope;
    return ast_accop;
  }
  return ast_variable;
}

ast_T* parse_nil(parser_T* p, scope_T* scope) {
  ast_T* ast_nil = init_ast(AST_NULL);
  ast_nil->scope = scope;
  return ast_nil;
}



ast_T* parse_term(parser_T* p, scope_T* scope) {
  ast_T* ast_left = parse_expr(p, scope);
  if(p->curToken->kind == Lbrack) {
    return ast_left;
  }
  next_token(p);
  // Asterisk
  while (
    curTokenIs(p, Asterisk) ||
    curTokenIs(p, Slash)
  ) {
    ast_T* ast_binop = init_ast(AST_BINOP);
    ast_binop->left = ast_left; // 5
    ast_binop->op = p->curToken->kind; // *
    next_token(p);
    // 6
    ast_binop->right = parse_expr(p, scope); // 6
    next_token(p); // )
    ast_binop->scope = scope;
    return ast_binop;
  }
  
  return ast_left;
}

ast_T* parse_binop(parser_T* p, scope_T* scope) {
  ast_T* ast_left = parse_term(p, scope); // )
  while(
    curTokenIs(p, Plus) ||
    curTokenIs(p, Minus)
  ) {
    ast_T* ast_binop = init_ast(AST_BINOP);
    ast_binop->left = ast_left;
    ast_binop->op = p->curToken->kind;
    next_token(p);
    ast_binop->right = parse_binop(p, scope);
    ast_binop->scope = scope;
    return ast_binop;
  }

  while(
    curTokenIs(p, Lbrack) 
  ) {
    ast_T* ast_indop = init_ast(AST_INDEXOP);
    ast_indop->index_op_value = ast_left;
    next_token(p); // m
    ast_indop->index = parse_binop(p, scope);
    parser_eat(p, Rbrack);
    ast_indop->scope = scope;
    return ast_indop;
  }
  
  
  return ast_left;
}
ast_T* parse_return(parser_T* p, scope_T* scope) {
  ast_T* ret = init_ast(AST_STMT_RETURN);
  parser_eat(p, KW_Return);
  ret->return_value = parse_binop(p, scope);
  ret->scope = scope;
  next_token(p);
  return ret;
}
ast_T* parse_if_statement(parser_T* p, scope_T* scope) {
  ast_T* ast = init_ast(AST_IF);
  ast->scope = scope;
  parser_eat(p, KW_If);
  parser_eat(p, Lparen);
  ast->if_condition = parse_boolean_expr(p, scope);
  parser_eat(p, Rparen);
  parser_eat(p, Lbrace);
  ast->if_body = parse_statementss(p, scope);
  return ast;
}

ast_T* parse_match(parser_T* p, scope_T* scope) {
  ast_T* ast = init_ast(AST_MATCH);
  /*ast->scope = scope;
  parser_eat(p, KW_If);
  parser_eat(p, Lparen);
  ast->if_condition = parse_boolean_expr(p, scope);
  parser_eat(p, Rparen);
  parser_eat(p, Lbrace);
  ast->if_body = parse_statementss(p, scope);*/
  return ast;
}

ast_T* parse_for_statement(parser_T* p, scope_T* scope){
  ast_T* ast = init_ast(AST_FOR);
  ast->scope = scope;
  parser_eat(p, KW_For);
  ast->for_var_test = parse_ident(p, scope);
  parser_eat(p, Ident);
  parser_eat(p, KW_In);
  ast->for_iterator = parse_binop(p, scope);
  parser_eat(p, Lbrace);
  ast_T* compound = init_ast(AST_COMPOUND);
  compound->scope = scope;
  ast_T* ast_statement = parse_statement(p, scope);
  list_push(compound->children, ast_statement);

  while(!curTokenIs(p, Rbrace)) {
    if(curTokenIs(p, Rparen)) parser_eat(p, Rparen);
    if(curTokenIs(p, Semicolon)) parser_eat(p, Semicolon);
    ast_statement = parse_statement(p, scope);
    if(ast_statement) {
        list_push(compound->children, ast_statement);
    }
  }

  ast->for_body = compound;
  return ast;
}

ast_T* parse_while_statement(parser_T* p, scope_T* scope) {
  ast_T* ast = init_ast(AST_WHILE);
  parser_eat(p, KW_While);
  // check for Leftparenthesis
  parser_eat(p, Lparen);
  ast->while_condition = parse_boolean_expr(p, scope);
  parser_eat(p, Rparen);
  parser_eat(p, Lbrace);
  ast_T* compound = init_ast(AST_COMPOUND);
  compound->scope = scope;
  ast_T* ast_statement = parse_statement(p, scope);
  list_push(compound->children, ast_statement);
  while(!curTokenIs(p, Rbrace)) {
    if(curTokenIs(p, Rparen)) parser_eat(p, Rparen);
    if(curTokenIs(p, Semicolon)) parser_eat(p, Semicolon);
    ast_statement = parse_statement(p, scope);
    if(ast_statement) {
        list_push(compound->children, ast_statement);
    }
  }
  ast->while_body = compound;
   ast->scope = scope;
  return ast;
}

ast_T* parse_struct_statement(parser_T* p, scope_T* scope) {
  ast_T* ast = init_ast(AST_STRT);
  parser_eat(p, KW_Struct);
  ast->struct_name = parse_variable(p, scope);
  parser_eat(p, Ident);
  parser_eat(p, Lbrace);
  ast_T* compound = init_ast(AST_COMPOUND);
  compound->scope = scope;
  ast_T* ast_statement = parse_statement(p, scope);
  list_push(compound->children, ast_statement);
  while(!curTokenIs(p, END_OF_FILE)) {
    ast_statement = parse_statement(p, scope);
    if(ast_statement) {
        list_push(compound->children, ast_statement);
    }
    if(curTokenIs(p, Rparen)) parser_eat(p, Rparen);
    if(curTokenIs(p, Semicolon)) parser_eat(p, Semicolon);
  }
  ast->for_body = compound;
   ast->scope = scope;
  return ast;
}



ast_T* parse_boolean_expr(parser_T* p, scope_T* scope) {
  ast_T* ast = init_ast(AST_BOOL);
  ast_T* left;
  if(curTokenIs(p, KW_True) ||
    curTokenIs(p, KW_False)) {
    left = parse_bool(p, scope);
    next_token(p);
  } else {
    left = parse_binop(p, scope);
  }
  // check for true or false == left
  // true = 1, false = 0
  //operator
  int op = p->curToken->kind;
  if(op==Lbrace || op==Rparen) {
   fprintf(stderr, "Error: Boolean expression cannot be length 1\n");

    exit(1);
  }
  next_token(p);
  ast_T* right = parse_binop(p, scope);
  ast->boolean_left = left;
  ast->bop = op;
  ast->boolean_right = right; 
   ast->scope = scope;
  return ast;
}

ast_T* parse_break(parser_T* p, scope_T* scope) {
  ast_T* ast = init_ast(AST_BREAK);
  parser_eat(p, KW_Break);
   ast->scope = scope;
  return ast;
}
ast_T* parse_continue(parser_T* p, scope_T* scope) {
  ast_T* ast = init_ast(AST_CONTINUE);
  parser_eat(p, KW_Continue);
   ast->scope = scope;
  return ast;
}

ast_T* parse_variable_reassignment(parser_T* p, scope_T* scope) {
  ast_T* ast = init_ast(AST_VARIABLE_REASSIGNMENT);
  ast->variable_def_name = parse_variable(p, scope);
  parser_eat(p, Ident);
  ast->vop = p->curToken->kind;
  next_token(p);
  ast->variable_reassignment_value = parse_binop(p, scope);
  ast->scope = scope;
  return ast;
}

ast_T* parse_index_op(parser_T* p, scope_T* scope) {
  ast_T* index = init_ast(AST_INDEXOP);
  index->index_op_value = parse_binop(p, scope);
  index->index = parse_binop(p, scope);
  index->scope = scope;
  return index;
}

ast_T* parse_import(parser_T* p, scope_T* scope) {
  ast_T* ast = init_ast(AST_IMPORT);
  parser_eat(p, KW_Import);
  ast->import_file = parse_string(p, scope);
  parser_eat(p, String);
  return ast;
}