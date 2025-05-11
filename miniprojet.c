#include "parser.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

// TODO: minimal grid parser

typedef struct {
  int val;
  int zone_id;
} Cell;

// Only inner pipes. The outer frame does not count as pipes.
// typedef struct PipeMap {
//     int *vert_pipes;
//     int *hor_pipes;
// } PipeMap;

typedef struct Grid {
  int n;
  int m;
  Cell **cells;
  int *zone_sizes; // Indexed by zone_id
  int zones_n;
} Grid;

void init_grid_cfg(Grid *grid, Config *cfg) {
  grid->n = cfg->n;
  grid->m = cfg->m;
  grid->zones_n = cfg->zones_n;

  int *zone_sizes = calloc(cfg->zones_n, sizeof(int));
  assert(zone_sizes);

  Cell **cells = malloc(sizeof(Cell *) * grid->n);
  for (int i = 0; i < grid->n; i++) {
    cells[i] = malloc(sizeof(Cell) * cfg->m);
    assert(cells[i]);
    for (int j = 0; j < grid->m; j++) {
      int zone_id = cfg->zone_ids[i][j];
      Cell c = {.val = cfg->vals[i][j], .zone_id = zone_id};
      cells[i][j] = c;
      zone_sizes[zone_id]++;
    }
  }
  grid->cells = cells;
  grid->zone_sizes = zone_sizes;
}

int count_digits(int n) {
  if (n == 0)
    return 1;

  int count = 0;
  while (n != 0) {
    n /= 10;
    count++;
  }
  return count;
}

Grid *new_grid(Config *cfg) {
  Grid *g = malloc(sizeof(Grid));
  assert(g);

  init_grid_cfg(g, cfg);
  return g;
}

void draw_grid(Grid *g) {
  int n = g->n;
  int m = g->m;

  for (int i = 0; i < n; i++) {
    // Lignes inter-lignes de cellules
    for (int j = 0; j < m; j++) {
      int zid = g->cells[i][j].zone_id;
      int zid_up = (i > 0) ? g->cells[i - 1][j].zone_id : -1; // Special value

      printf("%s", (j == 0) ? ((i == 0) ? "┏" : "┣") : "");

      if (zid != zid_up) {
        printf("━━━━━%s", (j == m - 1) ? ((i == 0) ? "┓" : "┫") : "+");
      } else {
        printf("     %s", (j == m - 1) ? "┫" : "+");
      }
    }
    printf("\n");

    for (int j = 0; j < m; j++) {
      int zid = g->cells[i][j].zone_id;
      int zid_left = (j > 0) ? g->cells[i][j - 1].zone_id : -1; // Special value
      int dgts = count_digits(g->cells[i][j].val);
      int right = (5-dgts)/2;
      int left = 5-right-dgts;

      if (zid != zid_left) {
        printf("┃");
      } else {
        printf(" ");
      }

      for (int i = 0; i < left; i++) {
        printf(" ");
      }
      printf("%d", g->cells[i][j].val);
      for (int i = 0; i < right; i++) {
        printf(" ");
      }
    }
    printf("┃\n");
  }
  for (int j = 0; j < m; ++j) {
    printf("%s━━━━━", (j == 0) ? "┗" : "+");
  }
  printf("┛");
  printf("\n");
}

bool all_set_to(int *arr, size_t size, int comp) {
  for (int i = 0; i < size; i++) {
    // printf("%d, ", arr[i]);
    if (arr[i] != comp) {
      return false;
    }
  }
  return true;
}

bool is_ok_around(Grid *g, int i, int j) {
  int val = g->cells[i][j].val;
  int n = g->n;
  int m = g->m;
  if (i - 1 >= 0) {
    if (g->cells[i - 1][j].val == val) {
      return false;
    }
    if (j - 1 >= 0) {
      if (g->cells[i - 1][j - 1].val == val) {
        return false;
      }
    }
    if (j + 1 < m) {
      if (g->cells[i - 1][j + 1].val == val) {
        return false;
      }
    }
  }
  if (i + 1 < n) {
    if (g->cells[i + 1][j].val == val) {
      return false;
    }
    if (j - 1 >= 0) {
      if (g->cells[i + 1][j - 1].val == val) {
        return false;
      }
    }
    if (j + 1 < m) {
      if (g->cells[i + 1][j + 1].val == val) {
        return false;
      }
    }
  }

  if (j - 1 >= 0) {
    if (g->cells[i][j - 1].val == val) {
      return false;
    }
  }

  if (j + 1 < m) {
    if (g->cells[i][j + 1].val == val) {
      return false;
    }
  }

  return true;
}

bool is_sol(Grid *g) {
  // 1..n in each zone
  int zn = g->zones_n;
  for (int k = 0; k < zn; k++) { // k = zone id
    int zone_size = g->zone_sizes[k];
    int *vals_attained = calloc(zone_size, sizeof(int));
    assert(vals_attained);
    for (int i = 0; i < g->n; i++) {
      for (int j = 0; j < g->m; j++) {
        if (g->cells[i][j].zone_id == k) {
          int val = g->cells[i][j].val;
          if (val < 1 || val > zone_size) {
            return false;
          }
          vals_attained[val - 1] += 1;
        }
        if (!is_ok_around(g, i, j)) {
          return false;
        }
      }
    }
    if (!all_set_to(vals_attained, zone_size, 1)) {
      return false;
    }
    // printf("\n");
  }

  // grid checks

  return true;
}

void grid_free(Grid *g) {
  free(g->zone_sizes);
  for (int i = 0; i < g->n; i++) {
    free(g->cells[i]);
  }
  free(g->cells);
  free(g);
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: ./a.out [filename]");
    exit(EXIT_FAILURE);
  }

  Config *cfg = parse_to_cfg(argv[1]);

  // printf("%d, %d\n", cfg->n, cfg->m);
  // printf("%d\n", cfg->zones_n);
  // for (int i = 0; i < cfg->n; i++) {
  //   for (int j = 0; j < cfg->m; j++) {
  //     printf("%d, ", cfg->vals[i][j]);
  //   }
  // }
  // printf("\n");
  // for (int i = 0; i < cfg->n; i++) {
  //   for (int j = 0; j < cfg->m; j++) {
  //     printf("%d, ", cfg->zone_ids[i][j]);
  //   }
  // }
  // printf("\n");

  Grid *grid = new_grid(cfg);
  draw_grid(grid);
  // for (int i = 0; i < grid->zones_n; i++) {
  //   printf("%d, ", grid->zone_sizes[i]);
  // }
  printf("%d\n", is_sol(grid));
  cfg_free(cfg);
  return 0;
}