#include "result_list.h"

#include <stdlib.h>

result_list* result_list_new()
{
  result_list* new = malloc(sizeof(result_list));
  new->first = NULL;
  new->last = NULL;
  new->size = 0;
  return new;
}

void result_list_free(result_list* result)
{
  result_list_item* current = result->first;
  result_list_item* keep;

  while (current != NULL) {
    keep = current;
    current = current->next;
    free(keep);
  }

  free(result);
}

void result_list_append(result_list* result, result_list_item item)
{
  result_list_item* new = malloc(sizeof(result_list_item));

  if (new == NULL) {
    return;
  }

  new->next = NULL;
  new->position = item.position;

  if (result->first == NULL) {
    result->first = new;
    result->last = new;
  }
  else {
    result->last->next = new;
    result->last = new;
  }

  result->size += 1;
}
