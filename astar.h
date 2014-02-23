#ifndef __ASTAR_H
#define __ASTAR_H
#include "map.h"

void pathfinder_setup(int width, int height);
void pathfinder_shutdown();

int pathfinder(adv_map *map, int x1, int y1, int x2, int y2);

#endif
