#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define is_space(c) (isspace(c) || (c) == 0x90)

typedef struct line_t {
  int n_leading;
  int n_following; /* Number of characters following the leading whitespace */
} line_t;

static void usage(const char *execname) {
  printf("Usage: %s <file>\n", execname);
}

static _Bool read_line(FILE *fp, line_t *line) {
  int c;
  assert(fp && line);

  if (ferror(fp) || feof(fp))
    return false;

  memset(line, 0, sizeof(line_t));

  /* Count leading white space */
  while (((c = fgetc(fp)) != EOF) && !ferror(fp))
    if (is_space((char)c))
      ++line->n_leading;
    else
      break;

  /* Last line */
  if (c == EOF && line->n_leading == 0)
    return false;

  /* If c is on a newline, then we are done. */
  if (c == '\n' || c == '\r')
    return true;

  /* Count all characters until we reach a newline. */
  while (((c = fgetc(fp)) != EOF) && !ferror(fp))
    if (c == '\n' || c == '\r')
      return true;
    else
      ++line->n_following;

  return true;
}

static void draw_whitespace(int n) {
  for (int i = 0; i < n; ++i)
    fprintf(stdout, "0 0 0  ");
}

static void draw_data(int n) {
  for (int i = 0; i < n; ++i)
    fprintf(stdout, "255 255 255  ");
}

static void draw(FILE *fp) {
  int n_lines = 0, max_lines = 2048, max_length = 0;
  line_t *lines, line;

  if (!(lines = realloc(NULL, sizeof(line_t) * max_lines))) {
    fprintf(stderr, "Error allocating memory to store image.\n");
    exit(ENOMEM);
  }

  /* Load data */
  while (read_line(fp, &line)) {
    /* If we run out of space, alloc more */
    if (n_lines >= max_lines) {
      max_lines *= 2;
      if (!(lines = realloc(lines, sizeof(line_t) * max_lines))) {
        fprintf(stderr, "Out of memory to store image.\n");
        exit(ENOMEM);
      }
    }

    lines[n_lines++] = line;

    if ((line.n_leading + line.n_following) > max_length)
      max_length = line.n_leading + line.n_following;
  }

  /* Header */
  fprintf(stdout, "P3\n%d\t%d\n255\n", max_length, n_lines);
  for (int i = 0; i < n_lines; ++i) {
    const line_t line = lines[i];
    const int rem = max_length - line.n_leading - line.n_following;
    draw_whitespace(line.n_leading);
    draw_data(line.n_following);
    draw_whitespace(rem);
    fputc('\n', stdout);
  }
}

int main(int argc, char **argv) {
  FILE *fp;
  const char *fname;

  if (argc != 2) {
    usage(argv[0]);
    exit(EXIT_FAILURE);
  }

  fname = argv[1];
  if (!(fp = fopen(fname, "r"))) {
    fprintf(stderr, "Error opening %s (%s)\n", fname, strerror(errno));
    exit(EXIT_FAILURE);
  }

  draw(fp);
  fclose(fp);

  return 0;
}
