#include "parser.h"
#include <stdio.h>


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
      int zid_up = (i > 0) ? g->cells[i-1][j].zone_id : -1; // Special value

      printf("%s", (j==0) ? ((i == 0) ? "┏":"┣") : "");

      if (zid != zid_up) {
        printf("━━━%s", (j==m-1)?((i == 0) ? "┓":"┫"):"+");
      } else {
        printf("   %s", (j==m-1)?"┫":"+");
      }
    }
    printf("\n");

    for (int j = 0; j < m; j++) {
      int zid = g->cells[i][j].zone_id;
      int zid_left = (j > 0) ? g->cells[i][j-1].zone_id : -1; // Special value

      if (zid != zid_left) {
        printf("┃");
      } else {
        printf(" ");
      }

      printf(" %d ", g->cells[i][j].val);
    }
    printf("┃\n");
  }
  for (int j = 0; j < m; ++j) {
    printf("%s━━━", (j == 0) ? "┗" : "+");
  }
  printf("┛");
  printf("\n");
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
  
  //printf("%d, %d\n", cfg->n, cfg->m);
  //printf("%d\n", cfg->zones_n);
  //for (int i = 0; i < cfg->n; i++) {
  //  for (int j = 0; j < cfg->m; j++) {
  //    printf("%d, ", cfg->vals[i][j]);
  //  }
  //}
  //printf("\n");
  //for (int i = 0; i < cfg->n; i++) {
  //  for (int j = 0; j < cfg->m; j++) {
  //    printf("%d, ", cfg->zone_ids[i][j]);
  //  }
  //}
  //printf("\n");

  Grid *grid = new_grid(cfg);
  draw_grid(grid);
  cfg_free(cfg);
  return 0;
}