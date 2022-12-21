#include "token.h"

char* keywords[] = {
  "if", "else", "true", "false", "func", "var", "while", "return", "for", "int", "float", "string", "boolean", "break", "continue", "struct", "enum"
,"in"};

int keywordi[] = {
  KW_If, KW_Else, KW_True, KW_False, KW_Func, KW_Var, KW_While, KW_Return, KW_For, KW_Int, KW_Float, KW_String, KW_Boolean, KW_Break, KW_Continue, KW_Struct, KW_Enum, KW_In
};
Token_t* init_token(char* text, enum TokenType kind) {
  Token_t* token = calloc(1, sizeof(struct TOKEN_STRUCT));
  token->text = text;
  token->kind = kind;
  return token;
}
int returnKeyword(char* text) {
  for(int i = 0; i < sizeof(keywords)/sizeof(keywords[0]); i++) {
    if(strcmp(text, keywords[i]) == 0) {
      return keywordi[i];
    }
  }
  return Ident;
}

char* typeToStr(enum TokenType kind) {
  switch(kind) {
    case Ident:
    return "Identifier";
    case Int:
    return "Int";
    case Float:
    return "Float";
    case String:
    return "String";
    case Colon:
    return ".";
    case Dot:
    return ":";
    case Comma:
    return ",";
    case Semicolon:
    return ";";
    case Plus:
    return "+";
    case Minus:
    return "-";
    case Asterisk:
    return "*";
    case Slash:
    return "/";
    case Lparen:
    return "(";
    case Rparen:
    return ")";
    case Lbrace:
    return "{";
    case Rbrace:
    return "}";
    case Lbrack:
    return "[";
    case Rbrack:
    return "]";
    case Eq:
    return "=";
    case Eqe:
    return "==";
    case Lt:
    return "<";
    case Gt:
    return ">";
   default:
    return "null";
  }
}