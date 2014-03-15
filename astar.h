#ifndef __ASTAR_H
#define __ASTAR_H
#include <stdint.h>

struct node;
typedef struct node{
	uint16_t x;
	uint16_t y;
	uint8_t cost;
	struct node *parent; struct node *child;
	struct node *next;
	struct node *prev;
}node_t, path_node_t;


struct t_bin_heap_t;
typedef struct t_pathfinder_info {
	int grid_width;
	int grid_height;
	
	/* callback that checks if x,y (in GRID coordinates) is walkable */
	int (*is_walkable)(int grid_x, int grid_y, void *monster_ptr);

	/* Internally used fields, leave them */
	struct t_bin_heap_t *open_heap;
	unsigned char *g_score, *h_score;
	unsigned char *node_type;
	node_t *top_node;
}pathfinder_info;

int pathfinder_setup(pathfinder_info *info);
node_t *pathfinder(pathfinder_info *info, void *monster_ptr, int x1, int y1, int x2, int y2);
void pathfinder_cleanup(pathfinder_info *info);
void pathfinder_free_path(node_t *path);


#endif
