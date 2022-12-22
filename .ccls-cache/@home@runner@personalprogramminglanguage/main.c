#include <stdio.h>
#include "io.h"
#include "lexer.h"
int main(void) {
  char* file_contents = read_file_contents("main.ppl");
  Lexer_t* l = init_lexer(file_contents);
  Token_t* t = next_token_lexer(l);
  while(t->kind != END_OF_FILE) {
    printf("Token(%s, %d)\n", t->text, t->kind);
    t = next_token_lexer(l);
  }
  return 0;
}