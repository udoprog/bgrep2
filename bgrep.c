#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <getopt.h>
#include <ctype.h>

#include "result_list.h"
#include "hexutils.h"

#define BUFFER_SIZE 0x1000

/*
 * define for portability reasons, size_t might not comply to the lx format.
 */
#ifndef HEXFORMAT
#  define HEXFORMAT "%08lx"
#endif

char* parse_pattern(const char* input, size_t* pattern_size) {
  char* pattern = malloc(0x100);
  size_t size = 0;

  while (*input != '\0')
  {
    if (*(input + 1) == '\0')
    {
      goto exit_error;
    }

    pattern[size++] = hextobyte(input);
    ++input;
    ++input;
  }

  if (pattern_size != NULL)
  {
    *pattern_size = size;
  }

  return pattern;
exit_error:
  if (pattern_size != NULL)
  {
    *pattern_size = 0;
  }

  free(pattern);
  return NULL;
}

/**
 * \brief Read the file into a memory buffer.
 *
 * \return A pointer to the newly created buffer on success.
 *         The buffer size will be made available in size unless it is null.
 *         The actual buffer size might differ.
 *
 * \return NULL on error
 *
 * \param file The file to move.
 * \param size The total size of the file when read.
 */
char* create_buffer(FILE* file, size_t* size)
{
  char* buffer = malloc(BUFFER_SIZE);
  char* buffer_tmp = NULL;
  size_t buffer_max = BUFFER_SIZE;
  size_t buffer_size = 0;
  int r;

  while ((r = fread(buffer + buffer_size, 1, BUFFER_SIZE, file)) > 0)
  {
    buffer_size += r;

    while (buffer_size + BUFFER_SIZE > buffer_max)
    {
      buffer_max *= 2;
      buffer_tmp = realloc(buffer, buffer_max);

      if (buffer_tmp == NULL)
      {
        goto error_free;
      }

      buffer = buffer_tmp;
    }
  }

  if (size != NULL) {
    *size = buffer_size;
  }

  return buffer;

error_free:
  free(buffer);

  if (size != NULL) {
    *size = buffer_size;
  }

  return NULL;
}

void find_pattern(const char* buffer, size_t buffer_size,
                  const char* pattern, size_t pattern_size,
                  result_list* result)
{

  size_t exact;
  int j;
  int all;

  if (buffer_size < pattern_size) {
    return;
  }

  for (exact = 0; exact < buffer_size - pattern_size; exact++) {
    all = 1;

    for (j = 0; j < pattern_size; j++) {
      if (buffer[exact+j] != pattern[j]) {
        all = 0;
        break;
      }
    }

    if (all == 1) {
      result_list_item item;
      item.position = exact;
      result_list_append(result, item);
    }
  }

  return;
}

typedef struct print_format {
  int group;
  int row;
  int context;
  int print_offset;
  int print_ascii;
} format;

void mark_position(format* fmt, FILE* file, size_t current, result_list_item* item)
{
  int print_offset = fmt->print_offset;
  int row          = fmt->row;
  int group        = fmt->group;

  size_t i;

  if (print_offset) {
    fprintf(file, "          ");
  }

  for (i = current - (current % row); i < current; i++) {
    if (i == item->position) {
      fprintf(file, "^-");
    } else {
      fprintf(file, "  ");
    }

    if ((i + 1) % group == 0) {
      fprintf(file, " ");
    }
  }

  fprintf(file, "\n");
}

void print_result_positions(
                  format* fmt, FILE* file,
                  result_list* result)
{
  result_list_item* item = result->first;

  while (item != NULL) {
    fprintf(file, "0x" HEXFORMAT "\n", item->position);
    item = item->next;
  }
}

void print_result(format* fmt, FILE* file,
                  const char* buffer, size_t buffer_size,
                  const char* pattern, size_t pattern_size,
                  result_list* result)
{
  int row = fmt->row;
  int group = fmt->group;
  int context = fmt->context;
  int print_offset = fmt->print_offset;
  int print_ascii = fmt->print_ascii;

  if (row < 4) {
    row = 4;
  }

  if (group < 4) {
    group = 4;
  }

  if (context < 0) {
    context = 0;
  }
  
  result_list_item* item = result->first;
  size_t floor;
  size_t ceil;
  size_t current;
  size_t next;
  size_t item_offset;
  size_t item_row;

  char ascii[row + 1 + group];
  int ascii_col = 0;

  char hexrow[row * 2 + group];
  int hexrow_col = 0;

  while (item != NULL) {
    item_offset = item->position % row;
    item_row = item->position / row;

    floor = item->position - item_offset;

    if (((int)floor) - row * context > 0) {
        floor -= row * context;
    }

    ceil  = item->position + pattern_size;

    if (ceil % row != 0) {
      ceil  = ceil + (row - (ceil % row));
    }

    ceil += row * context;

    if (ceil > buffer_size) {
      ceil = buffer_size;
    }

    if (floor < 0) {
      floor = 0;
    }

    fprintf(file, "==== "
                  "0x" HEXFORMAT " (0x" HEXFORMAT "-0x" HEXFORMAT ")"
                  " ====\n", item->position, floor, ceil);

    for (current = floor; current < ceil; current++)
    {
      next = current + 1;

      const char* h = bytetohex(buffer[current]);
      hexrow[hexrow_col++] = h[0];
      hexrow[hexrow_col++] = h[1];

      char buffer_current = buffer[current];

      int visible = isprint(buffer_current) && !isspace(buffer_current);

      ascii[ascii_col++] = visible ? buffer_current : '.';

      if (next % row != 0 && (next % group) == 0) {
        hexrow[hexrow_col++] = ' ';
        ascii[ascii_col++] = ' ';
      }

      if (next % row != 0 && next != ceil)
      {
        continue;
      }

      ascii[ascii_col] = '\0';
      ascii_col = 0;

      hexrow[hexrow_col] = '\0';
      hexrow_col = 0;

      if (print_offset) {
        fprintf(file, "0x" HEXFORMAT ": ", current - (current % row));
      }

      fprintf(file, "%s", hexrow);

      if (print_ascii)
      {
        fprintf(file, "    %s\n", ascii);
      }
      else
      {
        fprintf(file, "\n");
      }

      /* the current row is the one with the marked item on it */
      if (current / row == item_row)
      {
        mark_position(fmt, file, current, item);
      }
    }

    fprintf(file, "====\n");

    item = item->next;
  }
}

void print_usage(FILE* file)
{
  fprintf(file, "Usage: bgrep <pattern> [file]\n");
  fprintf(file, "Options:\n");
  fprintf(file, " -s         : Silent mode, only print locations\n");
  fprintf(file, " -r <row>   : Specify row size in bytes when printing\n");
  fprintf(file, " -g <group> : Specify grouping in bytes when printing\n");
  fprintf(file, " -x         : Specify pattern as hexadecimal notation\n");
  fprintf(file, "              i.e. 00ffde\n");
  fprintf(file, " -C <size>  : Include context around a match\n");
  fprintf(file, " -n         : Do not print offset\n");
  fprintf(file, " -a         : Do not print ascii\n");
  fprintf(file, " -h         : Print usage and exit\n");
}

int main(int argc, char* argv[])
{
  char*       buffer = NULL;
  size_t      buffer_size;

  format fmt;
  fmt.row = 0x10;
  fmt.group = 0x4;
  fmt.context = 0;
  fmt.print_offset = 1;
  fmt.print_ascii = 1;
  int only_positions = 0;

  int hex = 0;

  int c;

  while ((c = getopt(argc, argv, "r:g:xC:hnas")) != -1)
  {
    switch (c)
    {
      case 'r':
        fmt.row = atoi(optarg);
        break;
      case 'g':
        fmt.group = atoi(optarg);
        break;
      case 'C':
        fmt.context = atoi(optarg);
        break;
      case 'n':
        fmt.print_offset = 0;
        break;
      case 'a':
        fmt.print_ascii = 0;
        break;
      case 'x':
        hex = 1;
        break;
      case 's':
        only_positions = 1;
        break;
      case 'h':
        print_usage(stdout);
        break;
      case '?':
        if (optopt == 'c')
          fprintf (stderr, "Option -%c requires an argument.\n", optopt);
        else if (isprint (optopt))
          fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        else
          fprintf (stderr,
                   "Unknown option character `\\x%x'.\n",
                   optopt);
        return 1;
      default:
        abort();
    }
  }

  const char* input_pattern = NULL;
  const char* file_path = NULL;

  int index;
  int i = 0;

  for (index = optind; index < argc; index++)
  {
    switch (i++) {
    case 0:
      input_pattern = argv[index];
      break;
    case 1:
      file_path = argv[index];
      break;
    }
  }

  if (input_pattern == NULL) {
    print_usage(stdout);
    return 1;
  }

  char*         pattern = NULL;
  size_t        pattern_size;
  result_list*  result = NULL;
  FILE*         file = NULL;

  if (hex) {
    pattern = parse_pattern(input_pattern, &pattern_size);
  }
  else {
    pattern_size = strlen(input_pattern);
    pattern = malloc(pattern_size);
    strncpy(pattern, input_pattern, pattern_size);
  }

  if (pattern == NULL) {
    fprintf(stderr, "%s: Invalid input pattern: %s\n", argv[0], input_pattern);
    goto exit_error;
  }
  
  if (file_path == NULL || strncmp(file_path, "-", 2) == 0) {
    printf("using stdin\n");
    file = stdin;
  }
  else {
    file = fopen(file_path, "rb");
  }

  if (file == NULL) {
    fprintf(stderr, "%s: Failed to open file: %s\n", argv[0], strerror(errno));
    goto exit_error;
  }
  
  result = result_list_new();

  buffer = create_buffer(file, &buffer_size);

  if (buffer == NULL) {
    fprintf(stderr, "%s: Failed to allocate buffer\n", argv[0]);
    goto exit_error;
  }

  find_pattern(buffer, buffer_size, pattern, pattern_size, result);

  if (only_positions)
  {
    print_result_positions(&fmt, stdout, result);
  }
  else
  {
    print_result(&fmt, stdout, buffer, buffer_size, pattern, pattern_size, result);
  }

  free(pattern);
  free(buffer);
  result_list_free(result);
  fclose(file);
  return 0;

exit_error:
  if (pattern != NULL) {
    free(pattern);
  }

  if (result != NULL) {
    result_list_free(result);
  }

  if (file != NULL) {
    fclose(file);
  }

  return 1;
}
