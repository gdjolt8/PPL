#ifndef PPL_TOKEN_H
#define PPL_TOKEN_H

enum TokenType {
  END_OF_FILE,

  Ident,
  Int,
  Float,
  String,

  Plus,
  PlusEq,
  PlusPlus,
  Minus,
  MinusEq,
  MinusMinus,
  Asterisk,
  AsteriskEq,
  Slash,
  SlashEq,

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

  ArrowFunc,

  KW_If = 100,
  KW_Else,
  KW_True,
  KW_False,
  KW_Func,
  KW_Var,
  KW_While,
  KW_Return,
  KW_For,
  KW_Int,
  KW_Float,
  KW_String,
  KW_Boolean,
KW_Break,
KW_Continue,
KW_Struct,
KW_Enum,
KW_In
};
typedef struct TOKEN_STRUCT {
  char* text;
  enum TokenType kind;  
} Token_t;

Token_t* init_token(char* text, enum TokenType kind);
int returnKeyword(char* text);
char* typeToStr(enum TokenType kind);
#endif