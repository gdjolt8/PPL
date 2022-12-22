#ifndef PPL_LEXER_H
#define PPL_LEXER_H
#include "token.h"
typedef struct LEXER_STRUCT {
  char* src;
  unsigned int pos;
  char cur;

  int line;
  int column;
} Lexer_t;

Lexer_t* init_lexer(char* src);
void advance_lexer(Lexer_t* l);
char peek_lexer(Lexer_t* l);
void skip_whitespace_lexer(Lexer_t* l);
void skip_comment_lexer(Lexer_t* l);

Token_t* next_token_lexer(Lexer_t* l);
char* read_ident_lexer(Lexer_t* l);
char* read_int_lexer(Lexer_t* l);
char* read_string_lexer(Lexer_t* l);

Token_t* read_number_token_lexer(Lexer_t* l);
#endif