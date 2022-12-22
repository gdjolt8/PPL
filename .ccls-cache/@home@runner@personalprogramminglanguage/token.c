#include "token.h"

char* keywords[] = {
  "if", "else", "true", "false", "func", "var", "while", "for"
};

int keywordi[] = {
  KW_If, KW_Else, KW_True, KW_False, KW_Func, KW_Var, KW_While, KW_For
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