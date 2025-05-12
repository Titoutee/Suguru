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
      int right = (5 - dgts) / 2;
      int left = 5 - right - dgts;

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

bool _valid(Grid *g, int row, int col, int val) {
  int zone = g->cells[row][col].zone_id;

  // Vérifier doublon dans la zone
  for (int i = 0; i < g->n; i++) {
    for (int j = 0; j < g->m; j++) {
      if (g->cells[i][j].zone_id == zone && g->cells[i][j].val == val && (i != row || j != col))
        return false;
    }
  }

  // Vérifier les voisins (y compris diagonaux)
  for (int dr = -1; dr <= 1; dr++) {
    for (int dc = -1; dc <= 1; dc++) {
      int nr = row + dr;
      int nc = col + dc;
      if (nr >= 0 && nr < g->n && nc >= 0 && nc < g->m &&( dr != 0 || dc != 0)) {
        if (g->cells[nr][nc].val == val)
          return false;
      }
    }
  }

  return true;
}

bool valid(Grid *g) {
  for (int i = 0; i < g->n; i++) {
    for (int j = 0; j < g->m; j++) {
      if (g->cells[i][j].val != 0) {
        if (!_valid(g, i, j, g->cells[i][j].val)) {
          return false;
        }
      }
    }
  }
  return true;
}

bool solve(Grid *g, int i, int j) {
  if (i == g->n) {
    return true; // We found a solution which attained the end of the grid
  }

  int next_i = (j == g->m - 1) ? i + 1 : i;
  int next_j = (j + 1) % (g->m);

  if (g->cells[i][j].val != 0) {
    return solve(g, next_i, next_j);
  }

  int zid = g->cells[i][j].zone_id;
  int max_val_for_zone = g->zone_sizes[zid];

  for (int val = 1; val <= max_val_for_zone; val++) {
    if (_valid(g, i, j, val)) {
      g->cells[i][j].val = val;
      if (solve(g, next_i, next_j))
        return true;
      g->cells[i][j].val = 0;
    }
  }
  return false;
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
  printf("Initial state: \n");
  draw_grid(grid);
  printf("\n");
  
  if (!valid(grid)) {
    fprintf(stderr, "No solution was found for this config (early).\n");
    grid_free(grid);
    cfg_free(cfg);
    exit(EXIT_SUCCESS);
  }
  if (solve(grid, 0, 0)) {
    printf("A solution was found: \n");
    draw_grid(grid);
  } else {
    fprintf(stderr, "No solution was found for this config.\n");
  }
  grid_free(grid);
  cfg_free(cfg);
  return 0;
}