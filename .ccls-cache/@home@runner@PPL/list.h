#ifndef LIST_H
#define LIST_H
#include <stdlib.h>

typedef struct LIST_STRUCT {
  void** items;
  size_t used;
  size_t size;
} List;

List* init_list(size_t size);
void list_push(List* l, void* item);
#endif