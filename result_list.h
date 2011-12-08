#ifndef _RESULT_LIST_H_
#define _RESULT_LIST_H_

#include <stdlib.h>

typedef struct result_list_item {
  size_t position;
  struct result_list_item* next;
} result_list_item;

typedef struct {
  result_list_item* first;
  result_list_item* last;
  size_t size;
} result_list;

/**
 * \brief Allocate a new result list.
 */
result_list* result_list_new();

/**
 * \brief Free all memory associated with a result list.
 * \param result The result list to free.
 */
void result_list_free(result_list* result);

/**
 * \brief Append an entry to the result list.
 * \param result The result list to append to.
 * \param item The item to append.
 */
void result_list_append(result_list* result, result_list_item item);

#endif /* _RESULT_LIST_H_ */
