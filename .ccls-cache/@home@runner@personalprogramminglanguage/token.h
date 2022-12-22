#ifndef PPL_TOKEN_H
#define PPL_TOKEN_H

enum TokenType {
  END_OF_FILE,

  Ident,
  Int,
  Float,
  String,

  Plus,
  Minus,
  Asterisk,
  Slash,

  Lparen,
  Rparen,
  Lbrace,
  Rbrace,
  Lbrack,
  Rbrack,

  Colon,
  Semicolon,
  Comma,
  Dot,

  And,
  Not,
  Or,

  Gt,
  Gte,
  Lt,
  Lte,
  Eq,
  Eqe,
  Neq,

  KW_If = 100,
  KW_Else,
  KW_True,
  KW_False,
  KW_Func,
  KW_Var,
  KW_While,
  KW_For
};
typedef struct TOKEN_STRUCT {
  char* text;
  enum TokenType kind;  
} Token_t;

Token_t* init_token(char* text, enum TokenType kind);
int returnKeyword(char* text);
#endif