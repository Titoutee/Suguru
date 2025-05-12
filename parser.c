// Grid shapes parser.
// Config files are formatted as follows:

// n m
// 1 0 0 -> values (*m)
// ... (*n)
// 0 0 1 -> shape IDs (*m)
// ... (*n)

#include "parser.h"

int max(int *arr[], size_t n, size_t m) {
  assert(arr && n && m);
  size_t i, j;
  int max = arr[0][0];

  for (i = 0; i < n; ++i) {
    for (j = 0; j < m; j++)
      if (arr[i][j] > max) {
        max = arr[i][j];
      }
  }
  return max;
}

int split_into_array(char *line, char *sep, char *tokens[],
                     int max_tokens) {
  int count = 0;
  char *token = strtok(line, sep);
  while (token != NULL && count < max_tokens) {
    tokens[count++] = token;
    token = strtok(NULL, sep);
  }
  // free(token);
  return count;
}

// Local
typedef Config Config;

void cfg_free(Config *cfg) {
  for (int i = 0; i < cfg->n; i++) {
    free(cfg->zone_ids[i]);
    free(cfg->vals[i]);
  }
  free(cfg->zone_ids);
  free(cfg->vals);
  free(cfg);
}

void parse_vals(FILE *fp, int **vals, int n, int m) {
  char *tokens[128];
  size_t _n;
  char *line = NULL;
  ssize_t read_bytes;

  for (int i = 0; i < n; i++) {
    if ((read_bytes = getline(&line, &_n, fp)) == -1) {
      fprintf(stderr, "Ill-formed config file (values)");
      exit(EXIT_FAILURE);
    }

    int c = split_into_array(line, " ", tokens, m);
    if (c != m) {
      fprintf(stderr, "Line %d: expected %d values, got %d\n", i, m, c);
      exit(EXIT_FAILURE);
    }
    for (int j = 0; j < m; j++) {
      vals[i][j] = atoi(tokens[j]);
    }
  }
  free(line);
}

void parse_zone_ids(FILE *fp, int **ids, int n, int m) {
  char *tokens[128];
  size_t _n;
  char *line = NULL;
  ssize_t read_bytes;

  for (int i = 0; i < n; i++) {
    if ((read_bytes = getline(&line, &_n, fp)) == -1) {
      fprintf(stderr, "Ill-formed config file (values)");
      exit(EXIT_FAILURE);
    }

    int c = split_into_array(line, " ", tokens, m);
    if (c != m) {
      fprintf(stderr, "Line %d: expected %d values, got %d\n", i, m, c);
      exit(EXIT_FAILURE);
    }
    for (int j = 0; j < m; j++) {
      ids[i][j] = atoi(tokens[j]);
    }
  }
  free(line);
}

Config *parse_to_cfg(char *path) {
  Config *cfg = malloc(sizeof(Config));
  assert(cfg != NULL);

  FILE *fp = fopen(path, "r");
  assert(fp != NULL);

  char *line = NULL;
  ssize_t read_bytes;
  size_t _n;

  if ((read_bytes = getline(&line, &_n, fp)) == -1) {
    fprintf(stderr, "Ill-formed config file (should have found \"n m\")");
    exit(EXIT_FAILURE);
  }

  int n, m;

  sscanf(line, "%d %d\n", &n, &m);
  // printf("%d, %d", n, m);

  int **vals = malloc(sizeof(int *) * n);
  int **zone_ids = malloc(sizeof(int *) * n);
  assert(vals && zone_ids);
  for (int i = 0; i < n; i++) {
    vals[i] = malloc(sizeof(int) * m);
    zone_ids[i] = malloc(sizeof(int) * m);
    assert(vals[i] && zone_ids[i]);
  }

  parse_vals(fp, vals, n, m);
  parse_zone_ids(fp, zone_ids, n, m);

  cfg->n = n;
  cfg->m = m;
  cfg->vals = vals;
  cfg->zone_ids = zone_ids;
  cfg->zones_n = max(zone_ids, n, m) + 1;

  free(line);
  fclose(fp);
  return cfg;
}