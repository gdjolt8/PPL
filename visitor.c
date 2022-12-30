#include "visitor.h"
#include "scope.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "io.h"
#include "lexer.h"
#include "parser.h"
#include <ctype.h>
#include "./modules/socket.h"

char** split(const char* str, char delimiter, int* num_tokens) {
    // Calculate the number of tokens in the string
    int numb_tokens = 0;
    const char *start = str;
    while (*start) {
        if (*start == delimiter) {
            numb_tokens++;
        }
        start++;
    }
    // Add 1 to the number of tokens to account for the last token
    numb_tokens++;

    // Allocate memory for the array of tokens
    char **tokens = (char **)malloc(numb_tokens * sizeof(char *));

    // Iterate through the string and split it into tokens
    int i = 0;
    start = str;
    while (*start) {
        const char *end = start;
        while (*end && *end != delimiter) {
            end++;
        }
        int token_length = end - start;
        tokens[i] = (char *)malloc((token_length + 1) * sizeof(char));
        memcpy(tokens[i], start, token_length);
        tokens[i][token_length] = '\0';
        i++;
        start = end;
        if (*start) {
            start++;
        }
    }

    // Set the number of tokens in the output variable
    *num_tokens = numb_tokens;

    return tokens;
}

visitor_T* init_visitor() {
  visitor_T* vs = calloc(1, sizeof(visitor_T));
  return vs;
}
char* toss(char c) {
  char* str = calloc(2, sizeof(char));
  str[0] = c;
  str[1] = '\0';
  return str;
}

ast_T* first_index(ast_T* ast) {
  // Check if type is iterable
  if(!(ast->type == AST_ARRAY 
    || ast->type == AST_STRING) ) {
    fprintf(stderr, "Type %s is not iterable\n", astt(ast->type));
    exit(1);
  }
    
  if(ast->type == AST_ARRAY) {
    if(ast->array_children->used == 0) {
      return NULL;
    }
    ast_T* first = ast->array_children->items[0];
    return first; 
  } else {
    if(strlen(ast->string_value) == 0) {
      return NULL;
    }
    ast_T* first = init_ast(AST_STRING);
    first->string_value = toss(ast->string_value[0]);
    return first;
  }
}
ast_T* visitor_visit_boolean_expr(visitor_T* visitor, ast_T* node) {
  ast_T* newop = init_ast(AST_BOOL);
  newop->boolean_left = visitor_visit(visitor, node->boolean_left);
  if(!node->boolean_left) {
    newop->boolean_value = node->boolean_value;
    return newop;
  }
  newop->boolean_left = visitor_visit(visitor, node->boolean_left);
  newop->bop = node->bop;
  newop->boolean_right = visitor_visit(visitor, node->boolean_right);
  
  return newop;
}


ast_T* run_boolexpr(visitor_T* visitor, ast_T* node) {
  ast_T* left = visitor_visit(visitor, node->boolean_left);
  enum TokenType op = node->bop;
  ast_T* right = visitor_visit(visitor, node->boolean_right);
  
  ast_T* ast = init_ast(AST_BOOL);
  if(left->type == AST_BOOL && op == Rparen) {
    ast->boolean_value = left->boolean_value;
    return ast;
  }
  if(op == Eqe) {
    if(left->type == right->type) {
      switch(left->type) {
        case AST_INT:
          ast->boolean_value = left->int_value==right->int_value;
          break;
        case AST_FLOAT:
          ast->boolean_value = left->float_value==right->float_value;
          break;
        case AST_BOOL:
          ast->boolean_value = left->boolean_value==right->boolean_value;
          break;
        case AST_STRING:
          ast->boolean_value = strcmp(left->string_value, right->string_value) == 0;
          break;
        case AST_ARRAY:{
          int eq = 0;
          if(left->array_children->used != right->array_children->used) {
            ast->boolean_value = 0;
            break;
          }
          for(int i = 0; left->array_children->used; i++) {
            ast_T* child = (ast_T*)left->array_children->items[i];
            ast_T* ar_child = (ast_T*)right->array_children->items[i];
            if(child->type == ar_child->type) {
              switch(child->type) {
                case AST_INT:
                  eq += child->int_value==ar_child->int_value;
                  break;
                case AST_FLOAT:
                  eq += child->float_value==ar_child->float_value;
                  break;
                case AST_STRING:
                  eq += (strcmp(child->string_value, ar_child->string_value) == 0);
                  break;
                case AST_BOOL:
                  eq += child->boolean_value==ar_child->boolean_value;
                  break;
                }
            }
          }
          ast->boolean_value = (eq == left->array_children->used);
          break;
        }
      }
    }
    else {
      if(left->type == AST_INT && right->type== AST_FLOAT) {
        ast->boolean_value = left->int_value == left->float_value;
      } else {
       ast->boolean_value = 0;
      
      }
    }
  }
  else if(op == Lt) {
    if(left->type == right->type) {
      switch(left->type) {
        case AST_INT:
          ast->boolean_value = left->int_value < right->int_value;
          break;
        case AST_FLOAT:
          ast->boolean_value = left->float_value<right->float_value;
          break;
        case AST_BOOL:
          ast->boolean_value = left->boolean_value<right->boolean_value;
          break;
        default:
          if(left->type == AST_STRING) {
            fprintf(stderr, "Invalid operator '<' for strings\n");
            exit(1);
          }
          break;
      }
    }
  }
  else if(op == Lte) {
    if(left->type == right->type) {
      switch(left->type) {
        case AST_INT:
          ast->boolean_value = left->int_value <= right->int_value;
          break;
        case AST_FLOAT:
          ast->boolean_value = left->float_value <= right->float_value;
          break;
        case AST_BOOL:
          ast->boolean_value = left->boolean_value<=right->boolean_value;
          break;
        default:
          if(left->type == AST_STRING) {
            fprintf(stderr, "Invalid operator '<=' for strings\n");
            exit(1);
          }
          break;
      }
    }
  }
  else if(op == Gt) {
    if(left->type == right->type) {
      switch(left->type) {
        case AST_INT:
          ast->boolean_value = left->int_value>right->int_value;
          break;
        case AST_FLOAT:
          ast->boolean_value = left->float_value>right->float_value;
          break;
        case AST_BOOL:
          ast->boolean_value = left->boolean_value>right->boolean_value;
          break;
        default:
          if(left->type == AST_STRING) {
            fprintf(stderr, "Invalid operator '>' for strings\n");
            exit(1);
          }
          break;
      }
    }
  }
  else if(op == Gte) {
    if(left->type == right->type) {
      switch(left->type) {
        case AST_INT:
          ast->boolean_value = left->int_value>=right->int_value;
          break;
        case AST_FLOAT:
          ast->boolean_value = left->float_value>=right->float_value;
          break;
        case AST_BOOL:
          ast->boolean_value = left->boolean_value >= right->boolean_value;
          break;
        default:
          if(left->type == AST_STRING) {
            fprintf(stderr, "Invalid operator '>=' for strings");
            exit(1);
          }
          break;
      }
    }
  }
  else if(op == Neq) {
    if(left->type == right->type) {
      switch(left->type) {
        case AST_INT:
          ast->boolean_value = left->int_value!=right->int_value;
          break;
        case AST_FLOAT:
          ast->boolean_value = left->float_value!=right->float_value;
          break;
        case AST_STRING:
          ast->boolean_value = strcmp(left->string_value, right->string_value) != 0;
          break;
        case AST_BOOL:
          ast->boolean_value = left->boolean_value != right->boolean_value;
          break;
      }
    } else {
      ast->boolean_value = 0;
    }
  }
  else if(op == And) {
    if(left->type == right->type) {
      switch(left->type) {
        case AST_INT:
          ast->boolean_value = left->int_value&&right->int_value;
          break;
        case AST_FLOAT:
          ast->boolean_value = left->float_value&&right->float_value;
          break;
        case AST_STRING:
          ast->boolean_value = left->string_value && right->string_value;
          break;
        case AST_BOOL:
          ast->boolean_value = left->boolean_value && right->boolean_value;
          break;
      }
    }
  else if(op == Or) {
    if(left->type == right->type) {
      switch(left->type) {
        case AST_INT:
          ast->boolean_value = left->int_value || right->int_value;
          break;
        case AST_FLOAT:
          ast->boolean_value = left->float_value || right->float_value;
          break;
        case AST_STRING:
          ast->boolean_value = left->string_value || right->string_value;
          break;
        case AST_BOOL:
          ast->boolean_value = left->boolean_value || right->boolean_value;
          break;
      }
    }
  } else {
    fprintf(stderr, "Invalid operator for boolean expression\n");
    exit(1);
  }
  return ast;
}
  }
ast_T* run_ast(visitor_T* visitor, ast_T* ast) {
  ast_T* value;
  ast_T* left;
  enum TokenType op = ast->op;
  ast_T* right;
  left = visitor_visit(visitor, ast->left);
  
  right = visitor_visit(visitor, ast->right);

  
  
  if(left->type == AST_INT && right->type == AST_INT) {
    value = init_ast(AST_INT);
    if(op==Plus){
      value->int_value = left->int_value + right->int_value;
    }
    if(op==Minus){
      value->int_value = left->int_value - right->int_value;
    }
    if(op==Asterisk){
      value->int_value = left->int_value * right->int_value;
    }
    if(op==Slash){
      value->int_value = left->int_value / right->int_value;
    }
  }
  else if(left->type == AST_FLOAT && right->type == AST_FLOAT) {
    value = init_ast(AST_FLOAT);
    if(op==Plus){
      value->float_value = left->float_value + right->float_value;
    }
    if(op==Minus){
      value->float_value = left->float_value - right->float_value;
    }
    if(op==Asterisk){
      value->float_value = left->float_value * right->float_value;
    }
    if(op==Slash){
      value->float_value = left->float_value / right->float_value;
    }
  }
  else if(left->type == AST_STRING && right->type == AST_STRING) {
    value = init_ast(AST_STRING);
    if(op==Plus){
      char* buffer = (char*)calloc(strlen(left->string_value) + strlen(right->string_value) + 1, sizeof(char));
      strcat(buffer, left->string_value);
      strcat(buffer, right->string_value);
      value->string_value = buffer;
    }
    else {
      fprintf(stderr, "UnsupportedOperationError:  Cannot use operation '%s' with string and string\n", typeToStr(op));
      exit(1);
    }
  } else {
    fprintf(stderr, "UnsupportedOperationError:  Cannot use operation '%s' with %s and %s\n", typeToStr(op), astt(left->type), astt(right->type));
    exit(1);
  }
  return value;
}

ast_T* run_access(visitor_T* visitor, ast_T* ast) {
  ast_T* value;
  ast_T* left  = visitor_visit(visitor, ast->access_left);
  ast_T* right  = ast->access_right;

  if(left->type == AST_STRING) {
    if(right->type == AST_FUNCTION_CALL) {
      if(strcmp(right->function_call_name->ident_value, "upper") == 0) {
        char* new_value = (char*)calloc(strlen(left->string_value) + 1, sizeof(char));

        for(int i = 0; i < strlen(left->string_value); i++) {
          strcat(new_value, toss(toupper(left->string_value[i])));
        }
        value = init_ast(AST_STRING);
        value->string_value = new_value;
        return value;
      } 
      else if(strcmp(right->function_call_name->ident_value, "lower") == 0) {
        char* new_value = (char*)calloc(strlen(left->string_value) + 1, sizeof(char));

        for(int i = 0; i < strlen(left->string_value); i++) {
          strcat(new_value, toss(tolower(left->string_value[i])));
        }
        value = init_ast(AST_STRING);
        value->string_value = new_value;
        return value;
      } else if(strcmp(right->function_call_name->ident_value, "split") == 0) {
        ast_T* ast = init_ast(AST_ARRAY);
        if(right->function_call_args->used == 0) {
          for(int i = 0; i < strlen(left->string_value); i++) {
            ast_T* new_ast = init_ast(AST_STRING);
            new_ast->string_value = toss(left->string_value[i]);
            list_push(ast->array_children, new_ast);
          }
          return ast;
        }

        if(right->function_call_args->used > 1) {
          fprintf(stderr, "`split` function in string object, only has 0 or 1 arguments (got %zu)\n", right->function_call_args->used);
          exit(1);
        }
        ast_T* delim = visitor_visit(visitor, (ast_T*)right->function_call_args->items[0]);
        
        int number_tokens = 0;

        char** test = split(left->string_value, *delim->string_value, &number_tokens);

        for(int i = 0; i < number_tokens; i++) {
          ast_T* new_ast = init_ast(AST_STRING);
          new_ast->string_value = test[i];
          list_push(ast->array_children, new_ast);
        }
        return ast;
      }   else if(strcmp(right->function_call_name->ident_value, "substr") == 0) {
        visitor_expect_args(visitor, right->function_call_args, 2, (int[]){AST_INT, AST_INT});
        ast_T* from = visitor_visit(visitor, (ast_T*)right->function_call_args->items[0]);
        ast_T* to = visitor_visit(visitor, (ast_T*)right->function_call_args->items[1]);
        char* buf = (char*)calloc(to->int_value - from->int_value + 1, sizeof(char));
        for(int i = from->int_value; i < to->int_value; i++) {
          if(i == strlen(left->string_value)) {
            fprintf(stderr, "RangeError: index out of range.\n");
            exit(1);
          }
          strcat(buf, toss(left->string_value[i]));
        }
        ast_T* value = init_ast(AST_STRING);
        value->string_value = buf;
        return value;
      }
  }
  fprintf(stderr, "AttributeError: no function called '%s' in %s object\n", right->function_call_name->ident_value, astt(left->type));
  exit(1);
}
  }
ast_T* visitor_visit(visitor_T* visitor, ast_T* node) {
    if(!node) return init_ast(AST_NULL);
    
    switch (node->type)
    {
        case AST_VARIABLE_DEF: return visitor_visit_variable_def(visitor, node); break;
        case AST_FUNCTION_DEFINITION: return visitor_visit_function_def(visitor, node); break;
        case AST_VARIABLE: return visitor_visit_variable(visitor, node); break;
        case AST_FUNCTION_CALL: 
          return visitor_visit_function_call(visitor, node); break;
      case AST_STMT_RETURN: return visitor_visit_return(visitor, node); break;
      case AST_IDENT: return visitor_visit_ident(visitor, node);
        case AST_INT: return visitor_visit_int(visitor, node); break;
        case AST_FLOAT: return visitor_visit_float(visitor, node); break;
        case AST_STRING: return visitor_visit_string(visitor, node); break;
        case AST_BOOL: return visitor_visit_boolean_expr(visitor, node); break;
        case AST_ARRAY: 
           return visitor_visit_array(visitor, node); break;
        case AST_COMPOUND: return visitor_visit_compound(visitor, node); break;
      case AST_BINOP: return visitor_visit_binop(visitor, node);
      case AST_BREAK: return visitor_visit_break(visitor, node);
      case AST_CONTINUE: return visitor_visit_continue(visitor, node);
      case AST_IF: return visitor_visit_if(visitor, node);
      case AST_FOR: return visitor_visit_for(visitor, node);
      case AST_WHILE: return visitor_visit_while(visitor, node);
      case AST_VARIABLE_REASSIGNMENT: return visitor_visit_variable_reassignment(visitor, node);
      case AST_INDEXOP: return visitor_visit_indexop(visitor, node);
      case AST_MATCH: return visitor_visit_match(visitor, node);
      case AST_IMPORT: return visitor_visit_import(visitor, node);
      case AST_ACCESS: return run_access(visitor, node);
        case AST_NULL: return node; break;
    }
    fprintf(stderr, "Uncaught statement of type `%d`\n", node->type);
    exit(1);

    return init_ast(AST_NULL);
}
ast_T* visitor_visit_variable_def(visitor_T* visitor, ast_T* node) {
  node->variable_value = visitor_visit(visitor, node->variable_value);
  if(scope_get_var(node->scope, node->variable_def_name->ident_value)) {
    
  }
  scope_add_var(node->scope, node);
  return node;
}
ast_T* visitor_visit_function_def(visitor_T* visitor, ast_T* node) { 
  scope_add_func(node->scope, node);
  return node;
}
ast_T* visitor_visit_variable(visitor_T* visitor, ast_T* node) {
  ast_T* vdef = scope_get_var(node->scope, node->variable_name->ident_value);
  if(vdef != NULL) {
    return visitor_visit(visitor, vdef->variable_value);
  } else {
    fprintf(stderr, "Uncaught error: variable '%s' is not defined\n", node->variable_name->ident_value);
    exit(1);
  }
}
ast_T* visitor_visit_function_call(visitor_T* visitor, ast_T* node) {

  if(strcmp(node->function_call_name->ident_value, "println") == 0) {
    for(int i = 0; i < node->function_call_args->used; i++) {
      ast_T* visited_ast = visitor_visit(visitor, node->function_call_args->items[i]);
      switch(visited_ast->type) {
        case AST_STRING: printf("%s ", visited_ast->string_value);break;
        case AST_INT: printf("%lld ", visited_ast->int_value);break;
        case AST_FLOAT: printf("%Lf ", visited_ast->float_value);break;
        case AST_BOOL: printf("%s ", (visited_ast->boolean_value ? "true" : "false"));break;
        case AST_ARRAY: {
          printf("[");
          for(int i = 0; i < visited_ast->array_children->used; i++) {
            if(visited_ast->array_children->used-1 == i) {
              printf("%s", ast_to_string(visited_ast->array_children->items[i]));
            } else {
              printf("%s, ", ast_to_string(visited_ast->array_children->items[i]));
            }
          }
          printf("] ");
          break;
        }
        case AST_BINOP: {
          ast_T* ast_data = run_ast(visitor,visited_ast);
          if(ast_data->type == AST_INT) {
            printf("%llu ", ast_data->int_value);             
          }
          else if(ast_data->type == AST_FLOAT) {
            printf("%Lf ", ast_data->float_value);             
          } else {
          printf("%s ", ast_data->string_value);               
          }
        }
        case AST_NULL: printf("nil "); break;
      }
      
    }
    printf("\n");
    return init_ast(AST_NULL);
  }
  if(strcmp(node->function_call_name->ident_value, "print") == 0) {
    for(int i = 0; i < node->function_call_args->used; i++) {
      ast_T* visited_ast = visitor_visit(visitor, node->function_call_args->items[i]);
      switch(visited_ast->type) {
        case AST_STRING: printf("%s", visited_ast->string_value);break;
        case AST_INT: printf("%lld", visited_ast->int_value);break;
        case AST_FLOAT: printf("%Lf", visited_ast->float_value);break;
        case AST_BOOL: printf("%s", (visited_ast->boolean_value ? "true" : "false"));break;
        case AST_ARRAY: {
          printf("[");
          for(int i = 0; i < visited_ast->array_children->used; i++) {
            if(visited_ast->array_children->used-1 == i) {
              printf("%s", ast_to_string(visited_ast->array_children->items[i]));
            } else {
              printf("%s, ", ast_to_string(visited_ast->array_children->items[i]));
            }
          }
          printf("]");
          break;
        }
        case AST_BINOP: {
          ast_T* ast_data = run_ast(visitor,visited_ast);
          if(ast_data->type == AST_INT) {
            printf("%llu", ast_data->int_value);             
          }
          else if(ast_data->type == AST_FLOAT) {
            printf("%Lf", ast_data->float_value);             
          } else {
          printf("%s", ast_data->string_value);               
          }
        }
        case AST_NULL: printf("nil"); break;
      }
      
    }
    return init_ast(AST_NULL);
  }
  if(strcmp(node->function_call_name->ident_value, "pprintln") == 0) {
    visitor_expect_args(visitor, node->function_call_args, 1, (int[]){AST_ANY});
    ast_T* visited_ast = visitor_visit(visitor, node->function_call_args->items[0]);
    if(visited_ast->type == AST_INT){
      printf("0x%x\n", visited_ast->int_value);
    } else if(visited_ast->type == AST_BOOL){
      printf("0x%x\n", visited_ast->boolean_value);
    } else if(visited_ast->type == AST_FLOAT){
      printf("0x%x\n", visited_ast->float_value);
    } else if(visited_ast->type == AST_STRING) {
      printf("0x%x\n", visited_ast->string_value);
    } else {
      printf("0x%x\n", visited_ast);
    }
    return init_ast(AST_NULL);
  }
  if(strcmp(node->function_call_name->ident_value, "exit") == 0) {
    visitor_expect_args(visitor, node->function_call_args, 1, (int[]){AST_INT});
    ast_T* visited_ast = visitor_visit(visitor, node->function_call_args->items[0]);
    exit(visited_ast->int_value); 
    return init_ast(AST_NULL);
  }
  if(strcmp(node->function_call_name->ident_value, "randp") == 0) {
    visitor_expect_args(visitor, node->function_call_args, 0, (int[]){});
    srand(time(NULL)+rand()*rand()/rand()+100);
    ast_T* a = init_ast(AST_INT);
    a->int_value = rand() % 8080;
    return a;
  }
  if(strcmp(node->function_call_name->ident_value, "toi") == 0) {
    visitor_expect_args(visitor, node->function_call_args, 1, (int[]){AST_STRING});
    ast_T* visited_ast = visitor_visit(visitor, node->function_call_args->items[0]);
    int intVal = atoi(visited_ast->string_value);
    ast_T* a = init_ast(AST_INT);
    a->int_value = intVal;
    return a;
  }
  if(strcmp(node->function_call_name->ident_value, "tof") == 0) {
    visitor_expect_args(visitor, node->function_call_args, 1, (int[]){AST_STRING});
    ast_T* visited_ast = visitor_visit(visitor, node->function_call_args->items[0]);
    float floatVal = atof(visited_ast->string_value);
    ast_T* a = init_ast(AST_FLOAT);
    a->float_value = floatVal;
    return a;
  }
  if(strcmp(node->function_call_name->ident_value, "tos") == 0) {
    visitor_expect_args(visitor, node->function_call_args, 1, (int[]){AST_ANY});
    ast_T* visited_ast = visitor_visit(visitor, node->function_call_args->items[0]);
    if(visited_ast->type == AST_INT) {
      ast_T* a = init_ast(AST_STRING);
      a->string_value = (char*)calloc(100, sizeof(char));
      sprintf(a->string_value, "%llu", visited_ast->int_value);
      return a;
    } else if(visited_ast->type == AST_FLOAT) {
      ast_T* a = init_ast(AST_STRING);
      a->string_value = (char*)calloc(100, sizeof(char));
      sprintf(a->string_value, "%Lf", visited_ast->float_value);
      return a;
    } else if(visited_ast->type == AST_STRING) {
      ast_T* a = init_ast(AST_STRING);
      a->string_value = visited_ast->string_value;
      return a;
    } else if(visited_ast->type == AST_ARRAY) {
      ast_T* a = init_ast(AST_STRING);
      a->string_value = (char*)calloc(visited_ast->array_children->used+1, sizeof(char));
      for(int i = 0; i < visited_ast->array_children->used; i++) {
        a->string_value[i] = ((ast_T*)visited_ast->array_children->items[i])->int_value;
      }
      return a;
    }
  }
  if(strcmp(node->function_call_name->ident_value, "bytes") == 0) {
    visitor_expect_args(visitor, node->function_call_args, 1, (int[]){AST_STRING});
    ast_T* visited_ast = visitor_visit(visitor, node->function_call_args->items[0]);
    ast_T* a = init_ast(AST_ARRAY);
    for(int i = 0; i < strlen(visited_ast->string_value); i++) {
      ast_T* item = init_ast(AST_INT);
      item->int_value = (char)visited_ast->string_value[i];
      list_push(a->array_children, item);
    }
    return a;
  }
  if(strcmp(node->function_call_name->ident_value, "range") == 0) {
    visitor_expect_args(visitor, node->function_call_args, 1, (int[]){AST_INT});
    ast_T* visited_ast = visitor_visit(visitor, node->function_call_args->items[0]);
    ast_T* a = init_ast(AST_ARRAY);
    for(int i = 0; i < visited_ast->int_value; i++) {
      ast_T* item = init_ast(AST_INT);
      item->int_value = i;
      list_push(a->array_children, item);
    }
    return a;
  }
  if(strcmp(node->function_call_name->ident_value, "getln") == 0) {
    visitor_expect_args(visitor, node->function_call_args, 0, (int[]){});
    char* input = (char*)calloc(1000, sizeof(char));
    size_t max_input = 10000;
    getline(&input, &max_input, stdin);
    //fread(input, sizeof(char),max_input,stdin);
    input[strlen(input)-1] = '\0';
    ast_T* ast = init_ast(AST_STRING);
    ast->string_value = input;
    return ast;
  }
  if(strcmp(node->function_call_name->ident_value, "len") == 0) {
    visitor_expect_args(visitor, node->function_call_args, 1, (int[]){AST_ANY});
    ast_T* visited_ast = visitor_visit(visitor, node->function_call_args->items[0]);
    ast_T* ast = init_ast(AST_INT);
    if(visited_ast->type == AST_STRING ||
      visited_ast->type == AST_ARRAY) {
      if(visited_ast->type == AST_STRING) {
        ast->int_value = strlen(visited_ast->string_value);
      } else {
        ast->int_value = visited_ast->array_children->used;
      }
    } else {
      fprintf(stderr, "IterableError: Type: %s is not iterable\n", astt(visited_ast->type));
      exit(1);
    }
    return ast;
  }
  if(strcmp(node->function_call_name->ident_value, "chr") == 0) {
    visitor_expect_args(visitor, node->function_call_args, 1, (int[]){AST_INT});
    ast_T* visited_ast = visitor_visit(visitor, node->function_call_args->items[0]);

    char* size = (char*)calloc(2, sizeof(char));
    size[0] = (char)visited_ast->int_value;
    size[1] = '\0';
    ast_T* ast = init_ast(AST_STRING);
    ast->string_value = size;  
    return ast;
  }
  if(strcmp(node->function_call_name->ident_value, "ord") == 0) {
    visitor_expect_args(visitor, node->function_call_args, 1, (int[]){AST_STRING});
    ast_T* visited_ast = visitor_visit(visitor, node->function_call_args->items[0]);

    int size = *visited_ast->string_value;
    ast_T* ast = init_ast(AST_INT);
    ast->int_value = size;  
    return ast;
  }
  if(strcmp(node->function_call_name->ident_value, "assert") == 0) {
    visitor_expect_args(visitor, node->function_call_args, 1, (int[]){AST_BOOL});
    ast_T* visited_ast = visitor_visit(visitor, node->function_call_args->items[0]);
    if(visited_ast->boolean_value == 0) {
      fprintf(stderr, "AssertionError: Assertion failed\n");
      exit(1);
    }
     
    return init_ast(AST_NULL);
  }
  if(strcmp(node->function_call_name->ident_value, "execute") == 0) {
    visitor_expect_args(visitor, node->function_call_args, 1, (int[]){AST_STRING});
    ast_T* visited_ast = visitor_visit(visitor, node->function_call_args->items[0]);
    Lexer_t* l = init_lexer(visited_ast->string_value);
  parser_T* p = init_parser(l);
  ast_T* root = parser_parse(p, p->scope);
  visitor_T* visitor = init_visitor();
  visitor_visit(visitor, root);
    return init_ast(AST_NULL);
  }
  if(strcmp(node->function_call_name->ident_value, "fread") == 0) {
    visitor_expect_args(visitor, node->function_call_args, 1, (int[]){AST_STRING});
    ast_T* visited_ast = visitor_visit(visitor, node->function_call_args->items[0]);

    ast_T* strdata = init_ast(AST_STRING);
    strdata->string_value = read_file_contents(visited_ast->string_value);
    return strdata;
  }
  if(strcmp(node->function_call_name->ident_value, "freadb") == 0) {
    visitor_expect_args(visitor, node->function_call_args, 1, (int[]){AST_STRING});
    ast_T* visited_ast = visitor_visit(visitor, node->function_call_args->items[0]);

    ast_T* adata = init_ast(AST_ARRAY);
    char* file_data = read_file_contents(visited_ast->string_value);
    for(int i = 0; i < strlen(file_data); i++) {
      ast_T* byte = init_ast(AST_INT);
      byte->int_value = (int)file_data[i];
      list_push(adata->array_children, byte);
    }
    return adata;
  }
  if(strcmp(node->function_call_name->ident_value, "append") == 0) {
    visitor_expect_args(visitor, node->function_call_args, 2, (int[]){AST_ARRAY, AST_ANY});
    ast_T* visited_ast = visitor_visit(visitor, node->function_call_args->items[0]);
    ast_T* new_appendee = visitor_visit(visitor, node->function_call_args->items[1]);
    list_push(visited_ast->array_children, new_appendee);

    if(((ast_T*)node->function_call_args->items[0])->type == AST_VARIABLE) {
      int r = scope_get_varindex(node->scope, ((ast_T*)node->function_call_args->items[0])->variable_name->ident_value);
      ((ast_T*)node->scope->vars->items[r])->variable_value = visited_ast;
    } else {
      return init_ast(AST_NULL);
    }
    return init_ast(AST_NULL);
  }
  if(strcmp(node->function_call_name->ident_value, "array_extend") == 0) {
    visitor_expect_args(visitor, node->function_call_args, 3, (int[]){AST_ARRAY, AST_INT, AST_ANY});
    ast_T* visited_ast = visitor_visit(visitor, node->function_call_args->items[0]);
    ast_T* amt = visitor_visit(visitor, node->function_call_args->items[1]);
    ast_T* new_appendee = visitor_visit(visitor, node->function_call_args->items[2]);
    for(int i = 0; i < amt->int_value; i++) {
      list_push(visited_ast->array_children, new_appendee);
    }

    if(((ast_T*)node->function_call_args->items[0])->type == AST_VARIABLE) {
      int r = scope_get_varindex(node->scope, ((ast_T*)node->function_call_args->items[0])->variable_name->ident_value);
      ((ast_T*)node->scope->vars->items[r])->variable_value = visited_ast;
    } else {
      return init_ast(AST_NULL);
    }
    return init_ast(AST_NULL);
  }
  if(strcmp(node->function_call_name->ident_value, "socket_create") == 0) {
    visitor_expect_args(visitor, node->function_call_args, 3, (int[]){AST_INT, AST_INT, AST_INT});
    List* iargs = init_list(sizeof(ast_T*));
    for(int i = 0; i < 3; i++) {
      list_push(iargs, visitor_visit(visitor, node->function_call_args->items[i]));
    }
    return socket_socket_create(iargs);
  }
  if(strcmp(node->function_call_name->ident_value, "socket_bind") == 0) {
    visitor_expect_args(visitor, node->function_call_args, 3, (int[]){AST_INT, AST_STRING, AST_INT});
    List* iargs = init_list(sizeof(ast_T*));
    for(int i = 0; i < 3; i++) {
      list_push(iargs, visitor_visit(visitor, node->function_call_args->items[i]));
    }
    return socket_socket_bind(iargs);
  }
  if(strcmp(node->function_call_name->ident_value, "socket_listen") == 0) {
    visitor_expect_args(visitor, node->function_call_args, 2, (int[]){AST_INT, AST_INT});
    List* iargs = init_list(sizeof(ast_T*));
    for(int i = 0; i < 2; i++) {
      list_push(iargs, visitor_visit(visitor, node->function_call_args->items[i]));
    }
    return socket_socket_listen(iargs);
  }
  if(strcmp(node->function_call_name->ident_value, "socket_accept") == 0) {
    visitor_expect_args(visitor, node->function_call_args, 1, (int[]){AST_INT});
    List* iargs = init_list(sizeof(ast_T*));
    for(int i = 0; i < 1; i++) {
      list_push(iargs, visitor_visit(visitor, node->function_call_args->items[i]));
    }
    return socket_socket_accept(iargs);
  }
  if(strcmp(node->function_call_name->ident_value, "socket_read") == 0) {
    visitor_expect_args(visitor, node->function_call_args, 2, (int[]){AST_INT, AST_INT});
    List* iargs = init_list(sizeof(ast_T*));
    for(int i = 0; i < 2; i++) {
      list_push(iargs, visitor_visit(visitor, node->function_call_args->items[i]));
    }
    return socket_socket_read(iargs);
  }
  if(strcmp(node->function_call_name->ident_value, "socket_write") == 0) {
    visitor_expect_args(visitor, node->function_call_args, 3, (int[]){AST_INT, AST_STRING, AST_INT});
    List* iargs = init_list(sizeof(ast_T*));
    for(int i = 0; i < 3; i++) {
      list_push(iargs, visitor_visit(visitor, node->function_call_args->items[i]));
    }
    return socket_socket_write(iargs);
  }
  if(strcmp(node->function_call_name->ident_value, "socket_close") == 0) {
    visitor_expect_args(visitor, node->function_call_args, 1, (int[]){AST_INT});
    List* iargs = init_list(sizeof(ast_T*));
    for(int i = 0; i < 1; i++) {
      list_push(iargs, visitor_visit(visitor, node->function_call_args->items[i]));
    }
    return socket_socket_close(iargs);
  }
  
    
  
  ast_T* fdef = scope_get_func(node->scope, node->function_call_name->ident_value);
  if(fdef == NULL) {
    ast_T* vdef = scope_get_var(node->scope, node->function_call_name->ident_value);
    if(vdef != NULL) {
      fprintf(stderr, "Uncaught error: variable '%s' cannot be converted to function\n", node->function_call_name->ident_value);
      exit(1);
    }
    fprintf(stderr, "Uncaught error: method '%s' is not defined\n", node->function_call_name->ident_value);
    exit(1);
  }
  int regs[4];
  for(int i = 0; i < (int)node->function_call_args->used; i++) {
    regs[i] = AST_ANY;
  }
  visitor_expect_args(visitor, node->function_call_args, node->function_call_args->used, regs);
  ast_T* ast_var;
  ast_T* ast_value;
  ast_T* ast_vardef;
  ast_T* f;
  for(int i = 0; i < (int)node->function_call_args->used; i++) {
   ast_var = (ast_T*) fdef->function_definition_args->items[i];
    ast_value = (ast_T*) node->function_call_args->items[i];
    
   ast_vardef = init_ast(AST_VARIABLE_DEF);
    ast_vardef->variable_value = ast_value;
    ast_vardef->variable_def_name = init_ast(AST_IDENT);
    ast_vardef->variable_def_name->ident_value = (char*) calloc(strlen(ast_var->variable_name->ident_value) + 1, sizeof(char));
   strcpy(ast_vardef->variable_def_name->ident_value, ast_var->variable_name->ident_value);
    scope_add_var(fdef->function_definition_body->scope, ast_vardef);
  }
  f = visitor_visit(visitor, fdef->function_definition_body);
  /*for(int i = 0; i < fdef->function_definition_body->scope->vars->used; i++) {
    char* varname = ((ast_T*)fdef->function_definition_body->scope->vars->items[i])->variable_def_name->ident_value;
    scope_remove_var(fdef->function_definition_body->scope, varname);
  }*/
    return visitor_visit(visitor,fdef->function_definition_return_value);
}

ast_T* visitor_visit_return(visitor_T* visitor, ast_T* node) {
  ast_T* ast = init_ast(AST_STMT_RETURN);
  ast->return_value = visitor_visit(visitor, node->return_value);
  return ast;
}

ast_T* visitor_visit_binop(visitor_T* visitor, ast_T* node) {
  ast_T* new_binop = run_ast(visitor, node);
  return new_binop;
}
ast_T* visitor_visit_int(visitor_T* visitor, ast_T* node) {
  return node;
}
ast_T* visitor_visit_float(visitor_T* visitor, ast_T* node) {
  return node;
}
ast_T* visitor_visit_string(visitor_T* visitor, ast_T* node) {
  return node;
}
ast_T* visitor_visit_bool(visitor_T* visitor, ast_T* node) {
  ast_T* new_bool = init_ast(AST_BOOL);
  return node;
}
ast_T* visitor_visit_compound(visitor_T* visitor, ast_T* node) {
    for (int i = 0; i < node->children->used; i++)
    {
      ast_T* item=(ast_T*)node->children->items[i];
      visitor_visit(visitor, item);
    }

    return init_ast(AST_NULL);
}

ast_T* visitor_visit_if(visitor_T* visitor, ast_T* node) {
  ast_T* ast = init_ast(AST_IF);
  ast_T* st= node->if_condition;
  node->if_condition = run_boolexpr(visitor, st);
  if(node->if_condition->boolean_value) {
    for(int i = 0; i < node->if_body->children->used; i++) {
      ast_T* item = (ast_T*)node->if_body->children->items[i];
      visitor_visit(visitor, item);
    }
  }
  return ast;
}
ast_T* visitor_visit_for(visitor_T* visitor, ast_T* node) {
  ast_T* ast = init_ast(AST_FOR);
  ast->for_var_test = visitor_visit(visitor, node->for_var_test);
  ast->for_iterator = visitor_visit(visitor, node->for_iterator);
  ast_T* vardef = init_ast(AST_VARIABLE_DEF);
  vardef->variable_def_name = ast->for_var_test;
  vardef->variable_value = first_index(ast->for_iterator);
if(!vardef->variable_value) return init_ast(AST_NULL);
  scope_add_var(node->scope, vardef);
  int index = scope_get_varindex(node->scope, vardef->variable_def_name->ident_value);
  if(ast->for_iterator->type == AST_ARRAY) {
    for(int i = 0; i < ast->for_iterator->array_children->used; i++) {
      ast_T* child = (ast_T*)ast->for_iterator->array_children->items[i];
      vardef->variable_value = child;
      node->scope->vars->items[index] = vardef;
      // run code
      for(int j = 0; j < node->for_body->children->used; j++) {
        ast_T* line = (ast_T*)node->for_body->children->items[j];
      if(line->type == AST_BREAK) {
        break;
      }
      if(line->type == AST_CONTINUE) {
        continue;
      }
        visitor_visit(visitor, line);
        
      }
      scope_remove_var(node->scope, vardef->variable_def_name->ident_value);
    }
  }
  else if(ast->for_iterator->type == AST_STRING) {
    for(int i = 0; i < strlen(ast->for_iterator->string_value); i++) {
      ast_T* child = init_ast(AST_STRING);
      child->string_value = toss(ast->for_iterator->string_value[i]);
      vardef->variable_value = child;
      node->scope->vars->items[index] = vardef;
      for(int j = 0; j < node->for_body->children->used; j++) {
        ast_T* line = (ast_T*)node->for_body->children->items[j];
      if(line->type == AST_BREAK) {
        break;
      }
      if(line->type == AST_CONTINUE) {
        continue;
      }
        visitor_visit(visitor, line);
      }
      scope_remove_var(node->scope, vardef->variable_def_name->ident_value);

    }
  }
  else {
    fprintf(stderr, "IterableError: In for statement, item to be iterated over is not an array or a string\n");
    exit(1);
  }
  return ast;
}
ast_T* visitor_visit_while(visitor_T* visitor, ast_T* node) {
  ast_T* ast = init_ast(AST_WHILE);
  ast_T* st= node->while_condition;
  node->while_condition = run_boolexpr(visitor, st);
  node->while_body->scope = init_scope();
  while(node->while_condition->boolean_value) {
    for(int i = 0; i < node->while_body->children->used; i++) {
      ast_T* item = (ast_T*)node->while_body->children->items[i];
      if(item->type == AST_BREAK) {
        break;
      }
      if(item->type == AST_CONTINUE) {
        continue;
      }

      visitor_visit(visitor, item);
      for(int i = 0; i < node->while_body->scope->vars->used; i++) {
        scope_remove_var(node->while_body->scope, ((ast_T*)node->while_body->scope->vars->items[i])->variable_def_name->ident_value);
      }
    }
    // get both int value
    node->while_condition = run_boolexpr(visitor, st);
  }
  return ast;
}
ast_T* visitor_visit_array(visitor_T* visitor, ast_T* node) {
  ast_T* newarr = init_ast(AST_ARRAY);
  for (int i = 0; i < node->array_children->used; i++) {
    if(((ast_T*)node->array_children->items[i])->type == AST_ARRAY) {
      ast_T* aele = ((ast_T*)node->array_children->items[i]);
      //printf("%d, %d\n", aele->array_children->used, ((ast_T*)aele->array_children->items[0])->int_value);
    }
    list_push(newarr->array_children, visitor_visit(visitor, node->array_children->items[i]));
  }
  return newarr;
}

void visitor_expect_args(visitor_T* visitor, List* args, int argc, int arguments[]) {
    if (args->used != argc)
    {
        fprintf(stderr, "Expected %d arguments (got %zu)\n", (int) argc, args->used);
        exit(1);
    }
  unsigned int errors = 0;
  for(int i = 0; i < argc; i++) {
    ast_T* ast = (ast_T*)args->items[i];
    ast = visitor_visit(visitor, ast);
    if(ast->type != arguments[i]) {
      if(arguments[i] == AST_ANY) {
        break;
      }
      fprintf(stderr, "For argument #%d, expected type %s, but found type %s\n", i+1, astt(arguments[i]), astt(ast->type));
      errors += 1;
    }
    if(errors) {
      fprintf(stderr, "Previous errors, terminating!\n");
      exit(1);
    }
  }
}

ast_T* visitor_visit_break(visitor_T* visitor, ast_T* node) {
  return node;
}
ast_T* visitor_visit_continue(visitor_T* visitor, ast_T* node) {
    return node;
}

ast_T* visitor_visit_ident(visitor_T* visitor, ast_T* node) {
  return node;
}
ast_T* visitor_visit_variable_reassignment(visitor_T* visitor, ast_T* node) {
  ast_T* newv = init_ast(AST_VARIABLE_REASSIGNMENT);
  newv->variable_def_name = node->variable_def_name;
  newv->vop = node->vop;
  newv->variable_reassignment_value = visitor_visit(visitor, node->variable_reassignment_value);
  int ind = scope_get_varindex(node->scope, newv->variable_def_name->variable_name->ident_value);
  ast_T* new_var = scope_get_var(node->scope, newv->variable_def_name->variable_name->ident_value);
  if(new_var == NULL) {
    fprintf(stderr, "Oops: all null!");
    exit(1);
  }
  if(new_var->variable_const) {
    fprintf(stderr, "ConstError: Variable '%s' cannot be reassigned because it is declared constant\n\tTry: removing 'const' modifier on variable declaration for '%s'", newv->variable_def_name->variable_name->ident_value,newv->variable_def_name->variable_name->ident_value);
    exit(1);
  }
  if(newv->vop == PlusEq) {
    if(new_var->variable_value->type == AST_INT) {
      new_var->variable_value->int_value += newv->variable_reassignment_value->int_value;
    } else if(new_var->variable_value->type == AST_FLOAT) {
      new_var->variable_value->float_value += newv->variable_reassignment_value->float_value;
    } else if(new_var->variable_value->type == AST_STRING) {
      char* new_str = (char*)calloc(strlen(newv->variable_reassignment_value->string_value) + strlen(new_var->variable_value->string_value) + 1, sizeof(char));
      strcpy(new_str, new_var->variable_value->string_value);
      strcat(new_str, newv->variable_reassignment_value->string_value);
      new_var->variable_value->string_value = new_str;
    }
  } 
  if(newv->vop == MinusEq) {
    if(new_var->variable_value->type == AST_INT) {
      new_var->variable_value->int_value -= newv->variable_reassignment_value->int_value;
    } else if(new_var->variable_value->type == AST_FLOAT) {
      new_var->variable_value->float_value -= newv->variable_reassignment_value->float_value;
    } else if(new_var->variable_value->type == AST_STRING) {
      fprintf(stderr, "UnsupportedOperationError: cannot use operator '-' with string and string\n");
      exit(1);
    }
  }
  if(newv->vop == AsteriskEq) {
     if(new_var->variable_value->type == AST_INT) {
      new_var->variable_value->int_value *= newv->variable_reassignment_value->int_value;
    } else if(new_var->variable_value->type == AST_FLOAT) {
      new_var->variable_value->float_value *= newv->variable_reassignment_value->float_value;
    } else if(new_var->variable_value->type == AST_STRING) {
      fprintf(stderr, "UnsupportedOperationError: cannot use operator '*' with string and string\n");
      exit(1);
    }
  }
  if(newv->vop == SlashEq) {
     if(new_var->variable_value->type == AST_INT) {
      new_var->variable_value->int_value /= newv->variable_reassignment_value->int_value;
    } else if(new_var->variable_value->type == AST_FLOAT) {
      new_var->variable_value->float_value /= newv->variable_reassignment_value->float_value;
    } else if(new_var->variable_value->type == AST_STRING) {
      fprintf(stderr, "UnsupportedOperationError: cannot use operator '/' with string and string\n");
      exit(1);
    }
  }
  if(newv->vop == Eq) {
      new_var->variable_value = newv->variable_reassignment_value;
  }
  node->scope->vars->items[ind] = new_var;
  
  return newv;
}

ast_T* visitor_visit_indexop(visitor_T* visitor, ast_T* node) {
  ast_T* ast = init_ast(AST_INDEXOP);
  ast->index = visitor_visit(visitor, node->index);
  ast->index_op_value = visitor_visit(visitor, node->index_op_value);
  if(ast->index->type != AST_INT) {
    fprintf(stderr, "TypeError: cannot index a value without an integer value.\n");
    exit(1);
  }
  if(ast->index_op_value->type != AST_ARRAY && ast->index_op_value->type != AST_STRING) {
    // error
    fprintf(stderr, "TypeError: value to index is not an array or a string.\n");
    exit(1);
  }
  if(((ast_T*)ast->index_op_value)->type == AST_ARRAY) {
    if(ast->index->int_value >= ast->index_op_value->array_children->used) {
      fprintf(stderr, "RangeError: index out of range.\n");
      exit(1);
    }
  } else if(((ast_T*)ast->index_op_value)->type == AST_STRING) {
    if(ast->index->int_value >= strlen(ast->index_op_value->string_value)) {
    fprintf(stderr, "RangeError: index out of range.\n");
    exit(1);
  }
  }
  ast_T* as_ele;
  if(((ast_T*)ast->index_op_value)->type == AST_STRING)
  {
    ast_T* string = init_ast(AST_STRING);
    string->string_value = toss(((ast_T*)ast->index_op_value)->string_value[ast->index->int_value]);
    return string;
  } else if(((ast_T*)ast->index_op_value)->type == AST_ARRAY) { 
    return ((ast_T*)ast->index_op_value->array_children->items[ast->index->int_value]);
  }
  ast_T* ast_ele = init_ast(AST_NULL);
  ast_ele = (ast_T*)ast->index_op_value->array_children->items[ast->index->int_value];
  return ast_ele;
}

ast_T* visitor_visit_import(visitor_T* visitor, ast_T* node) {
  ast_T* visited_ast = visitor_visit(visitor, node->import_file);
  if(strstr(visited_ast->string_value, ".ppl") == NULL) {
    char* news = visited_ast->string_value;
    visited_ast->string_value = (char*)calloc(strlen(visited_ast->string_value)+4, sizeof(char));
    strcat(visited_ast->string_value, news);
    strcat(visited_ast->string_value, ".ppl");
  }
  char* data = read_file_contents(visited_ast->string_value);
  
  Lexer_t* l = init_lexer(data);
  
  parser_T* p = init_parser(l);
  ast_T* root = parser_parse(p, p->scope);
  visitor_T* vis = init_visitor();
  visitor_visit(vis, root);
  return node;
}

ast_T* visitor_visit_match(visitor_T* visitor, ast_T* node) {
  return node;
}