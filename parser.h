#ifndef PARSER_H_INCLUDED
#define PARSER_H_INCLUDED

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Config {
  int n;
  int m;
  int zones_n;
  int **zone_ids; // size: n * m
  int **vals;     // -1 = empty
};

typedef struct Config Config;
void cfg_free(Config *cfg);
Config *parse_to_cfg(char *path);

#endif
