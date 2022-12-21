#include "list.h"

List* init_list(size_t size) {
  List* list = calloc(1, sizeof(List));
  list->items = 0;
  list->used = 0;
  list->size = size;
  return list;
}
void list_push(List* l, void* item) {
  if(!l) {
    printf("ListNullException: List (%p) is null/not set.\n", l);
    return;
  }
  l->used += 1;
  if(!l->items) {
    l->items = calloc(1, sizeof(l->size));
  } else {
    l->items = realloc(l->items, (l->size * l->used));
  }
  l->items[l->used-1] = item;
}