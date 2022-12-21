#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
Lexer_t* init_lexer(char* src) {
  Lexer_t* l = calloc(1, sizeof(Lexer_t));
  l->src = src;
  l->pos = 0;
  l->cur = l->src[l->pos];
  l->line = 0;
  l->column = 0;
  return l;
}

char* tos(char c) {
  char* s = calloc(2,sizeof(char));
  s[0] = c;
  s[1] = '\0';
  return s;
}

void delChar(char** str, int idx) {
  char* word = *str;   
memmove(&word[idx], &word[idx + 1], strlen(word) - idx);
}
void advance_lexer(Lexer_t* l) {
  l->pos+=1;
  if(l->cur == '\n') {
    l->line++;
    l->column = 0;
  
  } 
  if(l->pos >= strlen(l->src)) {
    l->cur = '\0';
  }
  else {
    l->cur = l->src[l->pos];
  }
}
char peek_lexer(Lexer_t* l) {
  if(l->pos + 1 >= strlen(l->src)) 
    return '\0';
  return l->src[l->pos+1];
}
void skip_whitespace_lexer(Lexer_t* l) {
  while(l->cur == ' ' || l->cur == '\t' || l->cur == '\b' || l->cur == '\n') {
    advance_lexer(l);
  }
}
void skip_comment_lexer(Lexer_t* l) {
  if(l->cur == '/' && peek_lexer(l) == '/') {
    while(l->cur != '\n') {
      advance_lexer(l);
    }
    skip_whitespace_lexer(l);
  }
}

Token_t* next_token_lexer(Lexer_t* l) {
  skip_whitespace_lexer(l);
  skip_comment_lexer(l);
  if (l->cur == '/' && peek_lexer(l) == '*') {
    while(l->cur != '/') {
      advance_lexer(l);
    }
    advance_lexer(l);
    skip_whitespace_lexer(l);
  }
  Token_t* token = NULL;
  switch(l->cur) {
    case '+':
      if(peek_lexer(l) == '=') {
        char* buf = calloc(3, sizeof(char));
        buf[0] = l->cur;
        advance_lexer(l);
        buf[1] = l->cur;
        buf[2] = '\0';
        token = init_token(buf, PlusEq);
      } else if(peek_lexer(l) == '+') {
        char* buf = calloc(3, sizeof(char));
        buf[0] = l->cur;
        advance_lexer(l);
        buf[1] = l->cur;
        buf[2] = '\0';
        token = init_token(buf, PlusPlus);
      }else {
        token = init_token(tos(l->cur), Plus);
      }
      break;
    case '-':
      if(peek_lexer(l) == '=') {
        char* buf = calloc(3, sizeof(char));
        buf[0] = l->cur;
        advance_lexer(l);
        buf[1] = l->cur;
        buf[2] = '\0';
        token = init_token(buf, MinusEq);
      } else if(peek_lexer(l) == '-') {
        char* buf = calloc(3, sizeof(char));
        buf[0] = l->cur;
        advance_lexer(l);
        buf[1] = l->cur;
        buf[2] = '\0';
        token = init_token(buf, MinusMinus);
      } else {
        token = init_token(tos(l->cur), Minus);
      }
      break;
    case '*':
      if(peek_lexer(l) == '=') {
        char* buf = calloc(3, sizeof(char));
        buf[0] = l->cur;
        advance_lexer(l);
        buf[1] = l->cur;
        buf[2] = '\0';
        token = init_token(buf, AsteriskEq);
      } else {
        token = init_token(tos(l->cur), Asterisk);
      }
      break;
    case '/':
      if(peek_lexer(l) == '=') {
        char* buf = calloc(3, sizeof(char));
        buf[0] = l->cur;
        advance_lexer(l);
        buf[1] = l->cur;
        buf[2] = '\0';
        token = init_token(buf, SlashEq);
      } else {
        token = init_token(tos(l->cur), Slash);
      }
      break;
    case '(':
      token = init_token(tos(l->cur), Lparen);
      break;
    case ')':
      token = init_token(tos(l->cur), Rparen);
      break;
    case '{':
      token = init_token(tos(l->cur), Lbrace);
      break;
    case '}':
      token = init_token(tos(l->cur), Rbrace);
      break;
    case '[':
      token = init_token(tos(l->cur), Lbrack);
      break;
    case ']':
      token = init_token(tos(l->cur), Rbrack);
      break;
    case ':':
      token = init_token(tos(l->cur), Colon);
      break;
    case ',':
      token = init_token(tos(l->cur), Comma);
      break;
    case '.':
      token = init_token(tos(l->cur), Dot);
      break;
    case ';':
      token = init_token(tos(l->cur), Semicolon);
      break;
    case '>':
      if(peek_lexer(l) == '=') {
        char* buf = calloc(3, sizeof(char));
        buf[0] = l->cur;
        advance_lexer(l);
        buf[1] = l->cur;
        buf[2] = '\0';
        token = init_token(buf, Gte);
      } else {
        token = init_token(tos(l->cur), Gt);
      }
      break;
    case '<':
      if(peek_lexer(l) == '=') {
        char* buf = calloc(3, sizeof(char));
        buf[0] = l->cur;
        advance_lexer(l);
        buf[1] = l->cur;
        buf[2] = '\0';
        token = init_token(buf, Lte);
      } else {
        token = init_token(tos(l->cur), Lt);
      }
      break;
    case '=':
      if(peek_lexer(l) == '=') {
        char* buf = calloc(3, sizeof(char));
        buf[0] = l->cur;
        advance_lexer(l);
        buf[1] = l->cur;
        buf[2] = '\0';
        token = init_token(buf, Eqe);
      }else if(peek_lexer(l) == '>') {
        char* buf = calloc(3, sizeof(char));
        buf[0] = l->cur;
        advance_lexer(l);
        buf[1] = l->cur;
        buf[2] = '\0';
        token = init_token(buf, ArrowFunc);
      } else {
        token = init_token(tos(l->cur), Eq);
      }
      break;
    case '!':
      if(peek_lexer(l) == '=') {
        char* buf = calloc(3, sizeof(char));
        buf[0] = l->cur;
        advance_lexer(l);
        buf[1] = l->cur;
        buf[2] = '\0';
        token = init_token(buf, Neq);
      } else {
        token = init_token(tos(l->cur), Not);
      }
      break;
    case '&':
      if(peek_lexer(l) == '&') {
        char* buf = calloc(3, sizeof(char));
        buf[0] = l->cur;
        advance_lexer(l);
        buf[1] = l->cur;
        buf[2] = '\0';
        token = init_token(buf, And);
      } else {
        fprintf(stdout, "[Line %d Column %d] Expected '&&' got character: &%c\n", l->line, l->column, l->cur);
        exit(1);
      }
      break;
    case '|':
      if(peek_lexer(l) == '|') {
        char* buf = calloc(3, sizeof(char));
        buf[0] = l->cur;
        advance_lexer(l);
        buf[1] = l->cur;
        buf[2] = '\0';
        token = init_token(buf, Not);
      } else {
        fprintf(stdout, "[Line %d Column %d] Expected '||' got character: |%c\n", l->line, l->column, l->cur);
        exit(1);
      }
      break;
    case '\0':
      token = init_token("<EOF>", END_OF_FILE);
      break;
    default:
      if(isdigit(l->cur)) {
        token = read_number_token_lexer(l);
      }
      if(l->cur == '\"'||l->cur == '\'') {
        char* str = read_string_lexer(l);
        token = init_token(str, String);
      }
      if(isalpha(l->cur)) {
        char* str = read_ident_lexer(l);
        token = init_token(str, returnKeyword(str));
      }
      if(!token) {
        fprintf(stdout, "[Line %d Column %d] Unexpected character: %c\n", l->line, l->column, l->cur);
        exit(1);
      }
  }
  advance_lexer(l);
  return token;
}
char* read_ident_lexer(Lexer_t* l) {
  char* buf = calloc(1, sizeof(char));
  buf[0] = '\0';
  while(isalnum(peek_lexer(l)) || peek_lexer(l) == '_') {
    buf = realloc(buf, strlen(buf)+1);
    strcat(buf, tos(l->cur));
    advance_lexer(l);
  }
  strcat(buf, tos(l->cur));
  return buf;
}
char* read_int_lexer(Lexer_t* l) {
  char* buf = calloc(1, sizeof(char));
  buf[0] = '\0';
  while(isdigit(peek_lexer(l))) {
    buf = realloc(buf, strlen(buf)+1);
    strcat(buf, tos(l->cur));
    advance_lexer(l);
  }
  
  strcat(buf, tos(l->cur));
  return buf;
}
char* read_string_lexer(Lexer_t* l) {
  char quote = l->cur;
  advance_lexer(l);
  char* buf = calloc(1, sizeof(char));
  buf[0] = '\0';
  while(l->cur != quote) {
    if(l->cur == '\n') {
      fprintf(stdout, "[Line %d Column %d] String literal extends to newline\n", l->line, l->column);
      exit(1);
    }
    if(l->cur == 0){
      fprintf(stdout, "[Line %d Column %d] String literal extends to end of file\n", l->line, l->column);
      exit(1);
    }
    buf = realloc(buf, strlen(buf)+2);
    strcat(buf, tos(l->cur));
    advance_lexer(l);
  }
  for(int i = 0; i < strlen(buf); i++) {
    if(buf[i] == '\\') {
      if(buf[i+1] == 'n') {
        buf[i] = '\n';
        delChar(&buf, i+1);
      }
      else if(buf[i+1] == '\\') {
        buf[i] = '\\';
        delChar(&buf, i+1);
      }
      else if(buf[i+1] == 't') {
        buf[i] = '\t';
        delChar(&buf, i+1);
      }
      else if(buf[i+1] == 'r') {
        buf[i] = '\r';
        delChar(&buf, i+1);
      }
      else if(buf[i+1] == 'b') {
        buf[i] = '\b';
        delChar(&buf, i+1);
      }
      else if(buf[i+1] == '0') {
        buf[i] = 1;
        delChar(&buf, i+1);
      } else {
        fprintf(stdout, "[Line %d Column %d] Unexpected escape sequence: \'\\%c\'\n", l->line, l->column, buf[i+1]);
        exit(1);
      }
    }
  }
  return buf;
}

Token_t* read_number_token_lexer(Lexer_t* l) {
  char* intPart = read_int_lexer(l);
  if(peek_lexer(l) != '.') {
    return init_token(intPart, Int);
  }
  advance_lexer(l);
  char* fracPart = read_int_lexer(l);
  char* buf = calloc(strlen(intPart)+strlen(fracPart)+2, sizeof(char));
  sprintf(buf, "%s%s", intPart, fracPart);
  return init_token(buf, Float);
}