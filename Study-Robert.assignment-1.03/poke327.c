#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <sys/time.h>
#include <time.h>
#include <assert.h>

#include "heap.h"

#define malloc(size) ({          \
  void *_tmp;                    \
  assert((_tmp = malloc(size))); \
  _tmp;                          \
})

typedef struct path {
  heap_node_t *hn;
  uint8_t pos[2];
  uint8_t from[2];
  int32_t cost;
} path_t;

typedef enum dim {
  dim_x,
  dim_y,
  num_dims
} dim_t;

typedef uint8_t pair_t[num_dims];
pair_t characterLocate;


#define MAP_X              80
#define MAP_Y              21
#define MIN_TREES          10
#define MIN_BOULDERS       10
#define TREE_PROB          95
#define BOULDER_PROB       95
#define SHORT_MAX          300000

#define mappair(pair) (m->map[pair[dim_y]][pair[dim_x]])
#define mapxy(x, y) (m->map[y][x])
#define heightpair(pair) (m->height[pair[dim_y]][pair[dim_x]])
#define heightxy(x, y) (m->height[y][x])

void printDistanceMap(path_t pathNpc[MAP_Y][MAP_X]);

typedef enum __attribute__ ((__packed__)) terrain_type {
  ter_debug,
  ter_boulder,
  ter_tree,
  ter_path,
  ter_mart,
  ter_center,
  ter_grass,
  ter_clearing,
  ter_mountain,
  ter_forest,
  ter_water,
  ter_character // character with same naming conventions as other terrain
} terrain_type_t;

typedef struct map {
  terrain_type_t map[MAP_Y][MAP_X];
  uint8_t height[MAP_Y][MAP_X];
  uint8_t n, s, e, w;
} map_t;

typedef struct queue_node {
  int x, y;
  struct queue_node *next;
} queue_node_t;

static int32_t path_cmp(const void *key, const void *with) {
  return ((path_t *) key)->cost - ((path_t *) with)->cost;
}

static int32_t edge_penalty(uint8_t x, uint8_t y)
{
  return (x == 1 || y == 1 || x == MAP_X - 2 || y == MAP_Y - 2) ? 2 : 1;
}

static int32_t moveCost[4][12] = { // 2d array to hold info from 1.03 pdf
  //{INT_MAX, INT_MAX, 10, 50, 50, 15, 10, 15, 15, INT_MAX, 0}, // Hiker
  //{INT_MAX, INT_MAX, 10, 50, 50, 20, 10, INT_MAX, INT_MAX, INT_MAX, 0}, // Rival
  {SHORT_MAX, SHORT_MAX, 10, 50, 50, 15, 10, 15, 15, SHORT_MAX, 0}, // Hiker
  {SHORT_MAX, SHORT_MAX, 10, 50, 50, 20, 10, SHORT_MAX, SHORT_MAX, SHORT_MAX, 0}, // Rival
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // Swimmer, add later
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // Other, add later                                                                    
};

static void dijkstra_path(map_t *m, pair_t from, pair_t to)
{
  static path_t path[MAP_Y][MAP_X], *p;
  static uint32_t initialized = 0;
  heap_t h;
  uint32_t x, y;

  if (!initialized) {
    for (y = 0; y < MAP_Y; y++) {
      for (x = 0; x < MAP_X; x++) {
        path[y][x].pos[dim_y] = y;
        path[y][x].pos[dim_x] = x;
      }
    }
    initialized = 1;
  }
  
  for (y = 0; y < MAP_Y; y++) {
    for (x = 0; x < MAP_X; x++) {
      path[y][x].cost = SHORT_MAX;
    }
  }

  path[from[dim_y]][from[dim_x]].cost = 0;

  heap_init(&h, path_cmp, NULL);

  for (y = 1; y < MAP_Y - 1; y++) {
    for (x = 1; x < MAP_X - 1; x++) {
      path[y][x].hn = heap_insert(&h, &path[y][x]);
    }
  }

  while ((p = heap_remove_min(&h))) {
    p->hn = NULL;

    if ((p->pos[dim_y] == to[dim_y]) && p->pos[dim_x] == to[dim_x]) {
      for (x = to[dim_x], y = to[dim_y];
           (x != from[dim_x]) || (y != from[dim_y]);
           p = &path[y][x], x = p->from[dim_x], y = p->from[dim_y]) {
        mapxy(x, y) = ter_path;
        heightxy(x, y) = 0;
      }
      heap_delete(&h);
      return;
    }

    if ((path[p->pos[dim_y] - 1][p->pos[dim_x]].hn) && (path[p->pos[dim_y] - 1][p->pos[dim_x]].cost > ((p->cost + heightpair(p->pos)) * edge_penalty(p->pos[dim_x], p->pos[dim_y] - 1)))) {
      path[p->pos[dim_y] - 1][p->pos[dim_x]].cost = ((p->cost + heightpair(p->pos)) * edge_penalty(p->pos[dim_x], p->pos[dim_y] - 1));
      path[p->pos[dim_y] - 1][p->pos[dim_x]].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y] - 1][p->pos[dim_x]].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y] - 1][p->pos[dim_x]].hn);
    }
    if ((path[p->pos[dim_y]][p->pos[dim_x] - 1].hn) && (path[p->pos[dim_y]    ][p->pos[dim_x] - 1].cost > ((p->cost + heightpair(p->pos)) * edge_penalty(p->pos[dim_x] - 1, p->pos[dim_y])))) {
      path[p->pos[dim_y]][p->pos[dim_x] - 1].cost = ((p->cost + heightpair(p->pos)) * edge_penalty(p->pos[dim_x] - 1, p->pos[dim_y]));
      path[p->pos[dim_y]][p->pos[dim_x] - 1].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y]][p->pos[dim_x] - 1].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y]][p->pos[dim_x] - 1].hn);
    }
    if ((path[p->pos[dim_y]][p->pos[dim_x] + 1].hn) && (path[p->pos[dim_y]    ][p->pos[dim_x] + 1].cost >
      ((p->cost + heightpair(p->pos)) * edge_penalty(p->pos[dim_x] + 1, p->pos[dim_y])))) {
      path[p->pos[dim_y]][p->pos[dim_x] + 1].cost = ((p->cost + heightpair(p->pos)) * edge_penalty(p->pos[dim_x] + 1, p->pos[dim_y]));
      path[p->pos[dim_y]][p->pos[dim_x] + 1].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y]][p->pos[dim_x] + 1].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y]][p->pos[dim_x] + 1].hn);
    }
    if ((path[p->pos[dim_y] + 1][p->pos[dim_x]].hn) && (path[p->pos[dim_y] + 1][p->pos[dim_x]].cost >((p->cost + heightpair(p->pos)) * edge_penalty(p->pos[dim_x], p->pos[dim_y] + 1)))) {
      path[p->pos[dim_y] + 1][p->pos[dim_x]].cost = ((p->cost + heightpair(p->pos)) * edge_penalty(p->pos[dim_x], p->pos[dim_y] + 1));
      path[p->pos[dim_y] + 1][p->pos[dim_x]].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y] + 1][p->pos[dim_x]].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y] + 1][p->pos[dim_x]].hn);
    }
  }
}

void dijkstra_playerToNpc(map_t *m, pair_t from, int npcType)
{
  static path_t pathNpc[MAP_Y][MAP_X], *p; // these first 4 lines are unchanged from "dijkstra_path" besides path -> pathNpc
  static uint32_t initialized = 0;
  heap_t h;
  uint32_t x, y;

  if (!initialized) { // kept from "dijkstra_path"
    for (y = 0; y < MAP_Y; y++) {
      for (x = 0; x < MAP_X; x++) {
        pathNpc[y][x].pos[dim_y] = y;
        pathNpc[y][x].pos[dim_x] = x;
      }
    }
    initialized = 1;
  }
  
  for (y = 0; y < MAP_Y; y++) { // kept from "dijkstra_path"
    for (x = 0; x < MAP_X; x++) {
      pathNpc[y][x].cost = SHORT_MAX;
    }
  }

  pathNpc[from[dim_y]][from[dim_x]].cost = 0; // kept from "dijkstra_path"
  heap_init(&h, path_cmp, NULL);

  for (y = 1; y < MAP_Y - 1; y++) {// 1+- offset to exclude border terrain.
    for (x = 1; x < MAP_X - 1; x++) { 
      if(moveCost[npcType][m->map[y][x] - 1] != SHORT_MAX){  
        pathNpc[y][x].hn = heap_insert(&h, &pathNpc[y][x]);
      }else{
        pathNpc[y][x].hn = NULL;
      }
    }
  }

  while ((p = heap_remove_min(&h))) { //kept from "dijkstra_path"
    p -> hn = NULL;

    if (((pathNpc[p->pos[dim_y] - 1][p->pos[dim_x]].hn && pathNpc[p->pos[dim_y] - 1][p->pos[dim_x]].cost > ((p->cost + moveCost[npcType][m->map[p->pos[dim_y] - 1][p->pos[dim_x]] - 1]))))){
      pathNpc[p->pos[dim_y] - 1][p->pos[dim_x]].cost = ((p->cost + moveCost[npcType][m->map[p->pos[dim_y] - 1][p->pos[dim_x]] - 1]));
      heap_decrease_key_no_replace(&h, pathNpc[p->pos[dim_y] - 1][p->pos[dim_x]].hn);
    }
    if ((pathNpc[p->pos[dim_y]][p->pos[dim_x] - 1].hn) && (pathNpc[p->pos[dim_y]][p->pos[dim_x] - 1].cost >((p->cost + moveCost[npcType][m->map[p->pos[dim_y]][p->pos[dim_x] - 1] - 1])))) {
      pathNpc[p->pos[dim_y]][p->pos[dim_x] - 1].cost = ((p->cost + moveCost[npcType][m->map[p->pos[dim_y]][p->pos[dim_x] - 1] - 1]));
      heap_decrease_key_no_replace(&h, pathNpc[p->pos[dim_y]][p->pos[dim_x] - 1].hn);
    }
    if ((pathNpc[p->pos[dim_y] + 1][p->pos[dim_x]].hn) && (pathNpc[p->pos[dim_y] + 1][p->pos[dim_x]].cost > ((p->cost + moveCost[npcType][m->map[p->pos[dim_y] + 1][p->pos[dim_x]] - 1])))) {
      pathNpc[p->pos[dim_y] + 1][p->pos[dim_x]].cost = ((p->cost + moveCost[npcType][m->map[p->pos[dim_y] + 1][p->pos[dim_x]] - 1]));
      heap_decrease_key_no_replace(&h, pathNpc[p->pos[dim_y] + 1][p->pos[dim_x]].hn);
    }
    if ((pathNpc[p->pos[dim_y]][p->pos[dim_x] + 1].hn) && (pathNpc[p->pos[dim_y]][p->pos[dim_x] + 1].cost > ((p->cost + moveCost[npcType][m->map[p->pos[dim_y]][p->pos[dim_x] + 1] - 1])))) {
      pathNpc[p->pos[dim_y]][p->pos[dim_x] + 1].cost = ((p->cost + moveCost[npcType][m->map[p->pos[dim_y]][p->pos[dim_x] + 1] - 1]));
      heap_decrease_key_no_replace(&h, pathNpc[p->pos[dim_y]][p->pos[dim_x] + 1].hn);
    }
    if ((pathNpc[p->pos[dim_y] + 1][p->pos[dim_x] + 1].hn) && (pathNpc[p->pos[dim_y] + 1][p->pos[dim_x] + 1].cost > ((p->cost + moveCost[npcType][m->map[p->pos[dim_y] + 1][p->pos[dim_x] + 1] - 1])))) {
      pathNpc[p->pos[dim_y] + 1][p->pos[dim_x] + 1].cost = ((p->cost + moveCost[npcType][m->map[p->pos[dim_y] + 1][p->pos[dim_x] + 1] - 1]));
      heap_decrease_key_no_replace(&h, pathNpc[p->pos[dim_y] + 1][p->pos[dim_x] + 1].hn);
    }
    if ((pathNpc[p->pos[dim_y] + 1][p->pos[dim_x] - 1].hn) && (pathNpc[p->pos[dim_y] + 1][p->pos[dim_x] - 1].cost > ((p->cost + moveCost[npcType][m->map[p->pos[dim_y] + 1][p->pos[dim_x] - 1] - 1])))) {
      pathNpc[p->pos[dim_y] + 1][p->pos[dim_x] - 1].cost = ((p->cost + moveCost[npcType][m->map[p->pos[dim_y] + 1][p->pos[dim_x] - 1] - 1]));
      heap_decrease_key_no_replace(&h, pathNpc[p->pos[dim_y] + 1][p->pos[dim_x] - 1].hn);
    }
    if ((pathNpc[p->pos[dim_y] - 1][p->pos[dim_x] + 1].hn) && (pathNpc[p->pos[dim_y] - 1][p->pos[dim_x] + 1].cost > ((p->cost + moveCost[npcType][m->map[p->pos[dim_y] - 1][p->pos[dim_x] + 1] - 1])))) {
      pathNpc[p->pos[dim_y] - 1][p->pos[dim_x] + 1].cost = ((p->cost + moveCost[npcType][m->map[p->pos[dim_y] - 1][p->pos[dim_x] + 1] - 1]));
      heap_decrease_key_no_replace(&h, pathNpc[p->pos[dim_y] - 1][p->pos[dim_x] + 1].hn);
    }
    if ((pathNpc[p->pos[dim_y] - 1][p->pos[dim_x] - 1].hn) && (pathNpc[p->pos[dim_y] - 1][p->pos[dim_x] - 1].cost > ((p->cost + moveCost[npcType][m->map[p->pos[dim_y] - 1][p->pos[dim_x] - 1] - 1])))) {
      pathNpc[p->pos[dim_y] - 1][p->pos[dim_x] - 1].cost = ((p->cost + moveCost[npcType][m->map[p->pos[dim_y] - 1][p->pos[dim_x] - 1] - 1]));
      heap_decrease_key_no_replace(&h, pathNpc[p->pos[dim_y] - 1][p->pos[dim_x] - 1].hn);
    }
  }// while loop end

  printDistanceMap(pathNpc); // call to print distance maps
}

void printDistanceMap(path_t pathNpc[MAP_Y][MAP_X]) {
    int x, y;
    for(y = 0; y < MAP_Y; y++) {
        for(x = 0; x < MAP_X; x++) {
            if(y != 0 && x != 0 && y != MAP_Y - 1 && x != MAP_X - 1 && pathNpc[y][x].cost != SHORT_MAX) {
                if(pathNpc[y][x].cost % 100 == 0) {
                    printf("00 ");
                }else if(pathNpc[y][x].cost % 100 == 5){
                  printf("05 ");
                }else {
                    printf("%d ", (pathNpc[y][x].cost % 100)); 
                }
            } else {
                printf("   ");
            }
        }
        printf("\n");
    }
}

static int build_paths(map_t *m)
{
  pair_t from, to;

  /*  printf("%d %d %d %d\n", m->n, m->s, m->e, m->w);*/

  if (m->e != -1 && m->w != -1) {
    from[dim_x] = 1;
    to[dim_x] = MAP_X - 2;
    from[dim_y] = m->w;
    to[dim_y] = m->e;

    dijkstra_path(m, from, to);
  }

  if (m->n != -1 && m->s != -1) {
    from[dim_y] = 1;
    to[dim_y] = MAP_Y - 2;
    from[dim_x] = m->n;
    to[dim_x] = m->s;

    dijkstra_path(m, from, to);
  }

  if (m->e == -1) {
    if (m->s == -1) {
      from[dim_x] = 1;
      from[dim_y] = m->w;
      to[dim_x] = m->n;
      to[dim_y] = 1;
    } else {
      from[dim_x] = 1;
      from[dim_y] = m->w;
      to[dim_x] = m->s;
      to[dim_y] = MAP_Y - 2;
    }

    dijkstra_path(m, from, to);
  }

  if (m->w == -1) {
    if (m->s == -1) {
      from[dim_x] = MAP_X - 2;
      from[dim_y] = m->e;
      to[dim_x] = m->n;
      to[dim_y] = 1;
    } else {
      from[dim_x] = MAP_X - 2;
      from[dim_y] = m->e;
      to[dim_x] = m->s;
      to[dim_y] = MAP_Y - 2;
    }

    dijkstra_path(m, from, to);
  }

  if (m->n == -1) {
    if (m->e == -1) {
      from[dim_x] = 1;
      from[dim_y] = m->w;
      to[dim_x] = m->s;
      to[dim_y] = MAP_Y - 2;
    } else {
      from[dim_x] = MAP_X - 2;
      from[dim_y] = m->e;
      to[dim_x] = m->s;
      to[dim_y] = MAP_Y - 2;
    }

    dijkstra_path(m, from, to);
  }

  if (m->s == -1) {
    if (m->e == -1) {
      from[dim_x] = 1;
      from[dim_y] = m->w;
      to[dim_x] = m->n;
      to[dim_y] = 1;
    } else {
      from[dim_x] = MAP_X - 2;
      from[dim_y] = m->e;
      to[dim_x] = m->n;
      to[dim_y] = 1;
    }

    dijkstra_path(m, from, to);
  }

  return 0;
}

static int gaussian[5][5] = {
  {  1,  4,  7,  4,  1 },
  {  4, 16, 26, 16,  4 },
  {  7, 26, 41, 26,  7 },
  {  4, 16, 26, 16,  4 },
  {  1,  4,  7,  4,  1 }
};
static int smooth_height(map_t *m)
{
  int32_t i, x, y;
  int32_t s, t, p, q;
  queue_node_t *head, *tail, *tmp;
  /*  FILE *out;*/
  uint8_t height[MAP_Y][MAP_X];

  memset(&height, 0, sizeof (height));

  /* Seed with some values */
  for (i = 1; i < 255; i += 20) {
    do {
      x = rand() % MAP_X;
      y = rand() % MAP_Y;
    } while (height[y][x]);
    height[y][x] = i;
    if (i == 1) {
      head = tail = malloc(sizeof (*tail));
    } else {
      tail->next = malloc(sizeof (*tail));
      tail = tail->next;
    }
    tail->next = NULL;
    tail->x = x;
    tail->y = y;
  }

  /*
  out = fopen("seeded.pgm", "w");
  fprintf(out, "P5\n%u %u\n255\n", MAP_X, MAP_Y);
  fwrite(&height, sizeof (height), 1, out);
  fclose(out);
  */
  
  /* Diffuse the vaules to fill the space */
  while (head) {
    x = head->x;
    y = head->y;
    i = height[y][x];

    if (x - 1 >= 0 && y - 1 >= 0 && !height[y - 1][x - 1]) {
      height[y - 1][x - 1] = i;
      tail->next = malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x - 1;
      tail->y = y - 1;
    }
    if (x - 1 >= 0 && !height[y][x - 1]) {
      height[y][x - 1] = i;
      tail->next = malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x - 1;
      tail->y = y;
    }
    if (x - 1 >= 0 && y + 1 < MAP_Y && !height[y + 1][x - 1]) {
      height[y + 1][x - 1] = i;
      tail->next = malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x - 1;
      tail->y = y + 1;
    }
    if (y - 1 >= 0 && !height[y - 1][x]) {
      height[y - 1][x] = i;
      tail->next = malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x;
      tail->y = y - 1;
    }
    if (y + 1 < MAP_Y && !height[y + 1][x]) {
      height[y + 1][x] = i;
      tail->next = malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x;
      tail->y = y + 1;
    }
    if (x + 1 < MAP_X && y - 1 >= 0 && !height[y - 1][x + 1]) {
      height[y - 1][x + 1] = i;
      tail->next = malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x + 1;
      tail->y = y - 1;
    }
    if (x + 1 < MAP_X && !height[y][x + 1]) {
      height[y][x + 1] = i;
      tail->next = malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x + 1;
      tail->y = y;
    }
    if (x + 1 < MAP_X && y + 1 < MAP_Y && !height[y + 1][x + 1]) {
      height[y + 1][x + 1] = i;
      tail->next = malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x + 1;
      tail->y = y + 1;
    }

    tmp = head;
    head = head->next;
    free(tmp);
  }

  /* And smooth it a bit with a gaussian convolution */
  for (y = 0; y < MAP_Y; y++) {
    for (x = 0; x < MAP_X; x++) {
      for (s = t = p = 0; p < 5; p++) {
        for (q = 0; q < 5; q++) {
          if (y + (p - 2) >= 0 && y + (p - 2) < MAP_Y &&
              x + (q - 2) >= 0 && x + (q - 2) < MAP_X) {
            s += gaussian[p][q];
            t += height[y + (p - 2)][x + (q - 2)] * gaussian[p][q];
          }
        }
      }
      m->height[y][x] = t / s;
    }
  }
  /* Let's do it again, until it's smooth like Kenny G. */
  for (y = 0; y < MAP_Y; y++) {
    for (x = 0; x < MAP_X; x++) {
      for (s = t = p = 0; p < 5; p++) {
        for (q = 0; q < 5; q++) {
          if (y + (p - 2) >= 0 && y + (p - 2) < MAP_Y &&
              x + (q - 2) >= 0 && x + (q - 2) < MAP_X) {
            s += gaussian[p][q];
            t += height[y + (p - 2)][x + (q - 2)] * gaussian[p][q];
          }
        }
      }
      m->height[y][x] = t / s;
    }
  }

  /*
  out = fopen("diffused.pgm", "w");
  fprintf(out, "P5\n%u %u\n255\n", MAP_X, MAP_Y);
  fwrite(&height, sizeof (height), 1, out);
  fclose(out);

  out = fopen("smoothed.pgm", "w");
  fprintf(out, "P5\n%u %u\n255\n", MAP_X, MAP_Y);
  fwrite(&m->height, sizeof (m->height), 1, out);
  fclose(out);
  */

  return 0;
}

static void find_building_location(map_t *m, pair_t p)
{
  do {
    p[dim_x] = rand() % (MAP_X - 3) + 1;
    p[dim_y] = rand() % (MAP_Y - 3) + 1;

    if ((((mapxy(p[dim_x] - 1, p[dim_y]    ) == ter_path)     &&
          (mapxy(p[dim_x] - 1, p[dim_y] + 1) == ter_path))    ||
         ((mapxy(p[dim_x] + 2, p[dim_y]    ) == ter_path)     &&
          (mapxy(p[dim_x] + 2, p[dim_y] + 1) == ter_path))    ||
         ((mapxy(p[dim_x]    , p[dim_y] - 1) == ter_path)     &&
          (mapxy(p[dim_x] + 1, p[dim_y] - 1) == ter_path))    ||
         ((mapxy(p[dim_x]    , p[dim_y] + 2) == ter_path)     &&
          (mapxy(p[dim_x] + 1, p[dim_y] + 2) == ter_path)))   &&
        (((mapxy(p[dim_x]    , p[dim_y]    ) != ter_mart)     &&
          (mapxy(p[dim_x]    , p[dim_y]    ) != ter_center)   &&
          (mapxy(p[dim_x] + 1, p[dim_y]    ) != ter_mart)     &&
          (mapxy(p[dim_x] + 1, p[dim_y]    ) != ter_center)   &&
          (mapxy(p[dim_x]    , p[dim_y] + 1) != ter_mart)     &&
          (mapxy(p[dim_x]    , p[dim_y] + 1) != ter_center)   &&
          (mapxy(p[dim_x] + 1, p[dim_y] + 1) != ter_mart)     &&
          (mapxy(p[dim_x] + 1, p[dim_y] + 1) != ter_center))) &&
        (((mapxy(p[dim_x]    , p[dim_y]    ) != ter_path)     &&
          (mapxy(p[dim_x] + 1, p[dim_y]    ) != ter_path)     &&
          (mapxy(p[dim_x]    , p[dim_y] + 1) != ter_path)     &&
          (mapxy(p[dim_x] + 1, p[dim_y] + 1) != ter_path)))) {
          break;
    }
  } while (1);
}

static int place_pokemart(map_t *m)
{
  pair_t p;

  find_building_location(m, p);

  mapxy(p[dim_x]    , p[dim_y]    ) = ter_mart;
  mapxy(p[dim_x] + 1, p[dim_y]    ) = ter_mart;
  mapxy(p[dim_x]    , p[dim_y] + 1) = ter_mart;
  mapxy(p[dim_x] + 1, p[dim_y] + 1) = ter_mart;

  return 0;
}

static int place_center(map_t *m)
{  pair_t p;

  find_building_location(m, p);

  mapxy(p[dim_x]    , p[dim_y]    ) = ter_center;
  mapxy(p[dim_x] + 1, p[dim_y]    ) = ter_center;
  mapxy(p[dim_x]    , p[dim_y] + 1) = ter_center;
  mapxy(p[dim_x] + 1, p[dim_y] + 1) = ter_center;

  return 0;
}

/* Chooses tree or boulder for border cell.  Choice is biased by dominance *
 * of neighboring cells.                                                   */
static terrain_type_t border_type(map_t *m, int32_t x, int32_t y)
{
  int32_t p, q;
  int32_t r, t;
  int32_t miny, minx, maxy, maxx;
  
  r = t = 0;
  
  miny = y - 1 >= 0 ? y - 1 : 0;
  maxy = y + 1 <= MAP_Y ? y + 1: MAP_Y;
  minx = x - 1 >= 0 ? x - 1 : 0;
  maxx = x + 1 <= MAP_X ? x + 1: MAP_X;

  for (q = miny; q < maxy; q++) {
    for (p = minx; p < maxx; p++) {
      if (q != y || p != x) {
        if (m->map[q][p] == ter_mountain ||
            m->map[q][p] == ter_boulder) {
          r++;
        } else if (m->map[q][p] == ter_forest ||
                   m->map[q][p] == ter_tree) {
          t++;
        }
      }
    }
  }
  
  if (t == r) {
    return rand() & 1 ? ter_boulder : ter_tree;
  } else if (t > r) {
    if (rand() % 10) {
      return ter_tree;
    } else {
      return ter_boulder;
    }
  } else {
    if (rand() % 10) {
      return ter_boulder;
    } else {
      return ter_tree;
    }
  }
}

static int map_terrain(map_t *m, uint8_t n, uint8_t s, uint8_t e, uint8_t w)
{
  int32_t i, x, y;
  queue_node_t *head, *tail, *tmp;
  //  FILE *out;
  int num_grass, num_clearing, num_mountain, num_forest, num_water, num_total;
  terrain_type_t type;
  int added_current = 0;
  
  num_grass = rand() % 4 + 2;
  num_clearing = rand() % 4 + 2;
  num_mountain = rand() % 2 + 1;
  num_forest = rand() % 2 + 1;
  num_water = rand() % 2 + 1;
  num_total = num_grass + num_clearing + num_mountain + num_forest + num_water;

  memset(&m->map, 0, sizeof (m->map));

  /* Seed with some values */
  for (i = 0; i < num_total; i++) {
    do {
      x = rand() % MAP_X;
      y = rand() % MAP_Y;
    } while (m->map[y][x]);
    if (i == 0) {
      type = ter_grass;
    } else if (i == num_grass) {
      type = ter_clearing;
    } else if (i == num_grass + num_clearing) {
      type = ter_mountain;
    } else if (i == num_grass + num_clearing + num_mountain) {
      type = ter_forest;
    } else if (i == num_grass + num_clearing + num_mountain + num_forest) {
      type = ter_water;
    }
    m->map[y][x] = type;
    if (i == 0) {
      head = tail = malloc(sizeof (*tail));
    } else {
      tail->next = malloc(sizeof (*tail));
      tail = tail->next;
    }
    tail->next = NULL;
    tail->x = x;
    tail->y = y;
  }

  /*
  out = fopen("seeded.pgm", "w");
  fprintf(out, "P5\n%u %u\n255\n", MAP_X, MAP_Y);
  fwrite(&m->map, sizeof (m->map), 1, out);
  fclose(out);
  */
  
  /* Diffuse the vaules to fill the space */
  while (head) {
    x = head->x;
    y = head->y;
    i = m->map[y][x];
    
    if (x - 1 >= 0 && !m->map[y][x - 1]) {
      if ((rand() % 100) < 80) {
        m->map[y][x - 1] = i;
        tail->next = malloc(sizeof (*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x - 1;
        tail->y = y;
      } else if (!added_current) {
        added_current = 1;
        m->map[y][x] = i;
        tail->next = malloc(sizeof (*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x;
        tail->y = y;
      }
    }

    if (y - 1 >= 0 && !m->map[y - 1][x]) {
      if ((rand() % 100) < 20) {
        m->map[y - 1][x] = i;
        tail->next = malloc(sizeof (*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x;
        tail->y = y - 1;
      } else if (!added_current) {
        added_current = 1;
        m->map[y][x] = i;
        tail->next = malloc(sizeof (*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x;
        tail->y = y;
      }
    }

    if (y + 1 < MAP_Y && !m->map[y + 1][x]) {
      if ((rand() % 100) < 20) {
        m->map[y + 1][x] = i;
        tail->next = malloc(sizeof (*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x;
        tail->y = y + 1;
      } else if (!added_current) {
        added_current = 1;
        m->map[y][x] = i;
        tail->next = malloc(sizeof (*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x;
        tail->y = y;
      }
    }

    if (x + 1 < MAP_X && !m->map[y][x + 1]) {
      if ((rand() % 100) < 80) {
        m->map[y][x + 1] = i;
        tail->next = malloc(sizeof (*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x + 1;
        tail->y = y;
      } else if (!added_current) {
        added_current = 1;
        m->map[y][x] = i;
        tail->next = malloc(sizeof (*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x;
        tail->y = y;
      }
    }

    added_current = 0;
    tmp = head;
    head = head->next;
    free(tmp);
  }

  /*
  out = fopen("diffused.pgm", "w");
  fprintf(out, "P5\n%u %u\n255\n", MAP_X, MAP_Y);
  fwrite(&m->map, sizeof (m->map), 1, out);
  fclose(out);
  */
  
  for (y = 0; y < MAP_Y; y++) {
    for (x = 0; x < MAP_X; x++) {
      if (y == 0 || y == MAP_Y - 1 ||
          x == 0 || x == MAP_X - 1) {
        mapxy(x, y) = type = border_type(m, x, y);
      }
    }
  }

  m->n = n;
  m->s = s;
  m->e = e;
  m->w = w;

  mapxy(n,         0        ) = ter_path;
  mapxy(n,         1        ) = ter_path;
  mapxy(s,         MAP_Y - 1) = ter_path;
  mapxy(s,         MAP_Y - 2) = ter_path;
  mapxy(0,         w        ) = ter_path;
  mapxy(1,         w        ) = ter_path;
  mapxy(MAP_X - 1, e        ) = ter_path;
  mapxy(MAP_X - 2, e        ) = ter_path;

  return 0;
}

static int place_boulders(map_t *m)
{
  int i;
  int x, y;

  for (i = 0; i < MIN_BOULDERS || rand() % 100 < BOULDER_PROB; i++) {
    y = rand() % (MAP_Y - 2) + 1;
    x = rand() % (MAP_X - 2) + 1;
    if (m->map[y][x] != ter_forest && m->map[y][x] != ter_path) {
      m->map[y][x] = ter_boulder;
    }
  }

  return 0;
}

static int place_trees(map_t *m)
{
  int i;
  int x, y;
  
  for (i = 0; i < MIN_TREES || rand() % 100 < TREE_PROB; i++) {
    y = rand() % (MAP_Y - 2) + 1;
    x = rand() % (MAP_X - 2) + 1;
    if (m->map[y][x] != ter_mountain &&
        m->map[y][x] != ter_path     &&
        m->map[y][x] != ter_water) {
      m->map[y][x] = ter_tree;
    }
  }
  return 0;
}

static int place_pc(map_t *m){
  int y = 1 + rand() % (MAP_Y - 2);
  int i;
  int quit = 0;
    
  for (i = 1; i < MAP_X; ++i) {
    if (m->map[y][i] == ter_path && quit != 1) {
      m->map[y][i] = ter_character;
      characterLocate[dim_x] = i;
      characterLocate[dim_y] = y;
      quit = 1;
    }
  }
  return 0;
}

static int new_map(map_t *m)
{
  smooth_height(m);
  map_terrain(m,
              1 + rand() % (MAP_X - 2), 1 + rand() % (MAP_X - 2),
              1 + rand() % (MAP_Y - 2), 1 + rand() % (MAP_Y - 2));
  place_boulders(m);
  place_trees(m);
  build_paths(m);
  place_pokemart(m);
  place_center(m);
  place_pc(m);

  return 0;
}

static int world_maps_generate(map_t *m,  uint8_t n, uint8_t s, uint8_t e, uint8_t w, int x_dist, int y_dist){
  time_t t;
  srand((unsigned)time(&t));
  int prob_mart = rand() % 100 + 1;
  int prob_center = rand() % 100 + 1;

  int x = x_dist;
  int y = y_dist;

  if(x - 200 < 0){ // converts distance to coordinates
    x = (x - 200) * -1; 
  }else{
    x = x - 200;
  }
  if(y - 200 < 0){
    y = (y - 200) * -1;
  }else{
    y = y - 200;
  }

  int manhat_dist = x + y;
  int building_prob_funct = (((-45 * manhat_dist) / 200) + 50); // function from pdf
  smooth_height(m);
  map_terrain(m, n, s, e, w);

  if(x_dist == 0){
    m->map[w][0] = ter_boulder;
  }
  if(y_dist == 0){
    m->map[0][n] = ter_boulder;
  }
  if(x_dist == 400){
    m->map[e][79] = ter_boulder;
  }
  if(y_dist == 400){
    m->map[20][s] = ter_boulder;
  }

  place_boulders(m);
  place_trees(m);
  build_paths(m);

  if(prob_mart <= building_prob_funct){
    place_pokemart(m);
  }
  if(prob_center <= building_prob_funct){
    place_center(m);
  }

  place_pc(m);

  return 0;
}

static void print_map(map_t *m)
{
  int x, y;
  int default_reached = 0;
  
  for (y = 0; y < MAP_Y; y++) {
    for (x = 0; x < MAP_X; x++) {
      switch (m->map[y][x]) {
      case ter_boulder:
      case ter_mountain:
        putchar('%');
        break;
      case ter_tree:
      case ter_forest:
        putchar('^');
        break;
      case ter_path:
        putchar('#');
        break;
      case ter_mart:
        putchar('M');
        break;
      case ter_center:
        putchar('C');
        break;
      case ter_grass:
        putchar(':');
        break;
      case ter_clearing:
        putchar('.');
        break;
      case ter_water:
        putchar('~');
        break;
      case ter_character:
        putchar('@');
        break;
      default:
        default_reached = 1;
        break;
      }
    }
    putchar('\n');
  }

  if (default_reached) {
    fprintf(stderr, "Default reached in %s\n", __FUNCTION__);
  }
}

int main(int argc, char *argv[])
{
  map_t d;
  map_t *world_map[401][401]; // map for whole world
  
  int x_pos_cur = 200;
  int y_pos_cur = 200;
  char move_dir; // this will hold n s e w
  int exit = 0; // set to 1 at case q to quit game
  int fly_to_x;
  int fly_to_y;
  struct timeval tv;
  uint32_t seed;

  if (argc == 2) {
    seed = atoi(argv[1]);
  } else {
    gettimeofday(&tv, NULL);
    seed = (tv.tv_usec ^ (tv.tv_sec << 20)) & 0xffffffff;
  }

  int i, j;
  for(i = 0; i < 401; ++i){ // populate world with NULL maps
    for(j = 0; j < 401; ++j){
      world_map[i][j] = NULL;
    }
  }

  printf("Using seed: %u\n", seed);
  srand(seed);
  new_map(&d);
  print_map(&d);

  dijkstra_playerToNpc(&d, characterLocate, 0);
  dijkstra_playerToNpc(&d, characterLocate, 1);

  world_map[y_pos_cur][x_pos_cur] = malloc(sizeof(*world_map[y_pos_cur][x_pos_cur]));
  *(world_map[y_pos_cur][x_pos_cur]) = d;
  printf("Current location is: (0,0)\n");
  printf("Where would you like to go next? ");
  scanf(" %c", &move_dir);

  while(exit != 1){
    int n, s, e, w;
    map_t map_to_generate;

    switch(move_dir){ // switch between directions, fly and quit
      case 'n':
        if(y_pos_cur == 0){
          printf("You are currently at the north border of world and cannot go any further in this direction.\n");
          print_map(world_map[y_pos_cur][x_pos_cur]);
        }
        if(y_pos_cur != 0){
          if(world_map[y_pos_cur - 1][x_pos_cur] != NULL){
            y_pos_cur--;
            print_map(world_map[y_pos_cur][x_pos_cur]);
          }else{
            s = world_map[y_pos_cur][x_pos_cur] -> n;
            if(world_map[y_pos_cur - 1][x_pos_cur - 1] != NULL){
             w = world_map[y_pos_cur - 1][x_pos_cur - 1] -> e;
            }else{
              w = 1 + rand() % (MAP_Y - 2);
            }
            if(world_map[y_pos_cur - 1][x_pos_cur + 1] != NULL){
              e = world_map[y_pos_cur - 1][x_pos_cur + 1] -> w;
            }else{
              e = 1 + rand() % (MAP_Y - 2);
            }
            if(world_map[y_pos_cur - 2][x_pos_cur] != NULL && y_pos_cur > 1){
              n = world_map[y_pos_cur - 2][x_pos_cur] -> s;
            }else{
              n = 1 + rand() % (MAP_X - 2);
            }
            y_pos_cur--;
            world_maps_generate(&map_to_generate, n, s, e, w, x_pos_cur, y_pos_cur);
            world_map[y_pos_cur][x_pos_cur] = malloc(sizeof(*world_map[y_pos_cur][x_pos_cur])); // all the same for each direction
            *(world_map[y_pos_cur][x_pos_cur]) = map_to_generate;
            print_map(&map_to_generate);
          }
          dijkstra_playerToNpc(world_map[y_pos_cur][x_pos_cur], characterLocate , 0);
          dijkstra_playerToNpc(world_map[y_pos_cur][x_pos_cur], characterLocate, 1);
        }
      break;
      
      case 's':
        if(y_pos_cur == 400){
          printf("You are currently at the south border of world and cannot go any further in this direction.\n");
          print_map(world_map[y_pos_cur][x_pos_cur]);
        }
        if(y_pos_cur != 400){
          if(world_map[y_pos_cur + 1][x_pos_cur] != NULL){
            y_pos_cur++;
            print_map(world_map[y_pos_cur][x_pos_cur]);
          }else{
            n = world_map[y_pos_cur][x_pos_cur] -> s;
            if(world_map[y_pos_cur + 1][x_pos_cur - 1] != NULL){
             w = world_map[y_pos_cur + 1][x_pos_cur - 1] -> e;
            }else{
              w = 1 + rand() % (MAP_Y - 2);
            }
            if(world_map[y_pos_cur + 1][x_pos_cur + 1] != NULL){
              e = world_map[y_pos_cur + 1][x_pos_cur + 1] -> w;
            }else{
              e = 1 + rand() % (MAP_Y - 2);
            }
            if(world_map[y_pos_cur + 2][x_pos_cur] != NULL && y_pos_cur < 399){
              s = world_map[y_pos_cur + 2][x_pos_cur] -> n;
            }else{
              s = 1 + rand() % (MAP_X - 2);
            }
            y_pos_cur++;
            world_maps_generate(&map_to_generate, n, s, e, w, x_pos_cur, y_pos_cur);
            world_map[y_pos_cur][x_pos_cur] = malloc(sizeof(*world_map[y_pos_cur][x_pos_cur]));
            *(world_map[y_pos_cur][x_pos_cur]) = map_to_generate;
            print_map(&map_to_generate);
          }
          dijkstra_playerToNpc(world_map[y_pos_cur][x_pos_cur], characterLocate , 0);
          dijkstra_playerToNpc(world_map[y_pos_cur][x_pos_cur], characterLocate, 1);
        }
        break;

      case 'e':
        if(x_pos_cur == 400){
          printf("You are currently at the east border of world and cannot go any further in this direction..\n");
          print_map(world_map[y_pos_cur][x_pos_cur]);
        }
        if(x_pos_cur != 400){ 
          if(world_map[y_pos_cur][x_pos_cur + 1] != NULL){
            x_pos_cur++;
            print_map(world_map[y_pos_cur][x_pos_cur]);
          }else{
            w = world_map[y_pos_cur][x_pos_cur]->e;
            if(y_pos_cur != 0 && y_pos_cur != 400 && world_map[y_pos_cur - 1][x_pos_cur + 1] != NULL){
              n = world_map[y_pos_cur - 1][x_pos_cur + 1] -> s;
            }else{
              n = 1 + rand() % (MAP_X - 2);
            }
            if(y_pos_cur != 0 && y_pos_cur != 400 && world_map[y_pos_cur + 1][x_pos_cur + 1] != NULL){
              s = world_map[y_pos_cur + 1][x_pos_cur + 1] -> n;
            }else{
              s = 1 + rand() % (MAP_X - 2);
            }
            if(y_pos_cur != 0 && y_pos_cur != 400 && x_pos_cur < 399 && world_map[y_pos_cur][x_pos_cur + 2] != NULL){
              e = world_map[y_pos_cur + 2][x_pos_cur] -> w;
            }else{
              e = 1 + rand() % (MAP_Y - 2);
            }
            x_pos_cur++;
            world_maps_generate(&map_to_generate, n, s, e, w, x_pos_cur, y_pos_cur);
            world_map[y_pos_cur][x_pos_cur] = malloc(sizeof(*world_map[y_pos_cur][x_pos_cur]));
            *(world_map[y_pos_cur][x_pos_cur]) = map_to_generate;
            print_map(&map_to_generate);
          }
          dijkstra_playerToNpc(world_map[y_pos_cur][x_pos_cur], characterLocate , 0);
          dijkstra_playerToNpc(world_map[y_pos_cur][x_pos_cur], characterLocate, 1);
        }
      break;

      case 'w':
        if(x_pos_cur == 0){
        printf("You are currently at the west border of world and cannot go any further in this direction.\n");
        print_map(world_map[y_pos_cur][x_pos_cur]);
        }
        if(x_pos_cur != 0){
          if(world_map[y_pos_cur][x_pos_cur - 1] != NULL){
            x_pos_cur--;
            print_map(world_map[y_pos_cur][x_pos_cur]);
          }else{
            e = world_map[y_pos_cur][x_pos_cur]->w;
            if(y_pos_cur != 0 && y_pos_cur != 400 && world_map[y_pos_cur - 1][x_pos_cur - 1] != NULL){
            n = world_map[y_pos_cur - 1][x_pos_cur - 1]->s;
            }else{
              n = 1 + rand() % (MAP_X - 2);
            }
            if(y_pos_cur != 0 && y_pos_cur != 400 && world_map[y_pos_cur + 1][x_pos_cur - 1] != NULL){
              s = world_map[y_pos_cur + 1][x_pos_cur - 1] -> n;
            }else{
              s = 1 + rand() % (MAP_X - 2);
            }
            if(y_pos_cur != 0 && y_pos_cur != 400 && x_pos_cur > 1 && world_map[y_pos_cur][x_pos_cur - 2] != NULL){
              w = world_map[y_pos_cur + 2][x_pos_cur]->e;
            }else{
              w = 1 + rand() % (MAP_Y - 2);
            }
            x_pos_cur--;
            world_maps_generate(&map_to_generate, n, s, e, w, x_pos_cur, y_pos_cur);
            world_map[y_pos_cur][x_pos_cur] = malloc(sizeof(*world_map[y_pos_cur][x_pos_cur]));
            *(world_map[y_pos_cur][x_pos_cur]) = map_to_generate;
            print_map(&map_to_generate);
          }
          dijkstra_playerToNpc(world_map[y_pos_cur][x_pos_cur], characterLocate , 0);
          dijkstra_playerToNpc(world_map[y_pos_cur][x_pos_cur], characterLocate, 1);
        }
      break;

      case 'f':
        scanf(" %d %d", &fly_to_x, &fly_to_y);
        if(fly_to_x < -200 || fly_to_x > 200 || fly_to_y < -200 || fly_to_y > 200){
          printf("You have input coordinates that are beyond the limits of the game, please try again.\n");
          print_map(world_map[y_pos_cur][x_pos_cur]);
        }else{
        x_pos_cur = fly_to_x + 200;
        y_pos_cur = fly_to_y + 200;
        if(world_map[y_pos_cur][x_pos_cur] != NULL){
          print_map(world_map[y_pos_cur][x_pos_cur]);
        }else{
          if(x_pos_cur > 0 && world_map[y_pos_cur][x_pos_cur - 1] != NULL){
            e = world_map[y_pos_cur][x_pos_cur - 1]->w;        
          }else{
            e = 1 + rand() % (MAP_Y - 2);
          }
          if(x_pos_cur < 400 && world_map[y_pos_cur][x_pos_cur + 1] != NULL){
            w = world_map[y_pos_cur][x_pos_cur]->e; 
          }else{
            w = 1 + rand() % (MAP_Y - 2);
          }
          if(y_pos_cur > 0 && world_map[y_pos_cur - 1][x_pos_cur] != NULL){
            n = world_map[y_pos_cur - 1][x_pos_cur]->s;         
          }else{
            n = 1 + rand() % (MAP_X - 2);
          }
          if(y_pos_cur < 400 && world_map[y_pos_cur + 1][x_pos_cur] != NULL){
            s = world_map[y_pos_cur + 1][x_pos_cur]->n;
          }else{
            s = 1 + rand() % (MAP_X - 2);
          }
          world_maps_generate(&map_to_generate, n, s, e, w, x_pos_cur, y_pos_cur);
          world_map[y_pos_cur][x_pos_cur] = malloc(sizeof(*world_map[y_pos_cur][x_pos_cur]));
          *(world_map[y_pos_cur][x_pos_cur]) = map_to_generate;
          print_map(&map_to_generate);
        }
        dijkstra_playerToNpc(world_map[y_pos_cur][x_pos_cur], characterLocate , 0);
        dijkstra_playerToNpc(world_map[y_pos_cur][x_pos_cur], characterLocate, 1);
      }
      break;

      case 'q':
        exit = 1; // quit game 
      break;

      default:
        printf("You have input a command that doesn't exist, please try valid command.\n");
        print_map(world_map[y_pos_cur][x_pos_cur]);
        dijkstra_playerToNpc(world_map[y_pos_cur][x_pos_cur], characterLocate , 0);
        dijkstra_playerToNpc(world_map[y_pos_cur][x_pos_cur], characterLocate, 1);
      break;
    }
    if(exit != 1){
      printf("Current location is: (%d, %d)\n", x_pos_cur - 200, y_pos_cur - 200);
      printf("Where would you like to go next? ");
      scanf(" %c", &move_dir);
    }
  }
  return 0;
}