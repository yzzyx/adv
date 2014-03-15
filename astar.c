#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include "map.h"
#include "common.h"
#include "animation.h"
#include "gamestate.h"
#include "astar.h"

#undef DEBUG
#define _log(...)

#define TYPE_OPEN 1
#define TYPE_CLOSED 2

int mod_x[4] = { -1,  0, 1, 0 };
int mod_y[4] = {  0, -1, 0, 1 };

typedef struct t_bin_heap_t{
	node_t **data;
	int heap_size;
	int array_size;
}bin_heap_t;

int node_id = 0;

static node_t *
node_new_pos(pathfinder_info *info, int x, int y)
{
	node_t *new_node;

	new_node = calloc(1,sizeof *new_node);
	if (new_node == NULL)
		return NULL;

	new_node->x = x;
	new_node->y = y;

	/* Link all active nodes together, so we can clean them up later */
	new_node->next = info->top_node;
	info->top_node = new_node;
	if (new_node->next)
		new_node->next->prev = new_node;

	return new_node;
}

static int
manhattan(int x1, int y1, int x2, int y2)
{
	int abs_x, abs_y;

	if (x1  > x2) abs_x = x1  - x2;
	else abs_x = x2 - x1;

	if (y1 > y2) abs_y = y1 - y2;
	else abs_y = y2 - y1;

	return abs_x + abs_y;
}

static bin_heap_t *
bin_heap_new()
{
	bin_heap_t *bh;

	bh = malloc(sizeof(bin_heap_t));

	/* FIXME! 200 ? varför? */
	bh->data = malloc(sizeof(node_t*)* 200);
	bh->array_size = 200;
	bh->heap_size = 0;

	return bh;
}

static node_t *
bin_heap_get_min(bin_heap_t *bh)
{
	if (bh->heap_size == 0)
		return NULL;
	return bh->data[0];
}

static void
bin_heap_clear(bin_heap_t *bh)
{
	bh->heap_size = 0;
}

static void
bin_heap_free(bin_heap_t *bh)
{
	free(bh->data);
	free(bh);
}

#define bin_heap_left_child(bh,idx) 2*idx+1
#define bin_heap_right_child(bh,idx) 2*idx+2
#define bin_heap_parent(bh,idx) (idx-1)/2

static void
bin_heap_bubble_up(bin_heap_t *bh, int idx)
{
	int parent;
	node_t *tmp;
	while (idx != 0) {

		parent = bin_heap_parent(bh,idx);
		if (bh->data[parent]->cost > bh->data[idx]->cost) {
			tmp = bh->data[parent];
			bh->data[parent] = bh->data[idx];

			bh->data[idx] = tmp;

			idx = parent;
		}else
			break;
	}
}

static void
bin_heap_bubble_down(bin_heap_t *bh)
{
	int left, right;
	int child;
	int idx;
	node_t *tmp;

	idx = 0;
	for (;;) {
		left = bin_heap_left_child(bh,idx);
		right = bin_heap_right_child(bh,idx);

		/* We're at the bottom */
		if (left >= bh->heap_size && right >= bh->heap_size)
			return;

		/* One child */
		if (left >= bh->heap_size || right >= bh->heap_size) {
			child = (left > bh->heap_size) ? right : left;
		}else{
			/* Two children */
			if (bh->data[left]->cost < bh->data[right]->cost)
				child = left;
			else
				child = right;
		}

		if (bh->data[child]->cost < bh->data[idx]->cost) {
			tmp = bh->data[child];
			bh->data[child] = bh->data[idx];
			bh->data[idx] = tmp;

		}else
			return;

		idx = child;
	}
}

static void
bin_heap_add(bin_heap_t *bh, node_t *node)
{
	int idx;

	if (bh->heap_size == bh->array_size) {
		/* FIXME! - reallocate heap */
		printf("Binary heap overflow!");
		abort();
	}

	idx = bh->heap_size ++;
	bh->data[idx] = node;
	bin_heap_bubble_up(bh,idx);
}

static void
bin_heap_del_min(bin_heap_t *bh)
{
	bh->heap_size --;
	if (bh->heap_size == 0) {
		return;
	}

	bh->data[0] = bh->data[bh->heap_size];
	bin_heap_bubble_down(bh);
}

static int
find_node(bin_heap_t *bh, int x, int y)
{
	int i;

	for (i = 0; i < bh->heap_size; i++) {
		if (bh->data[i]->x == x &&
			bh->data[i]->y == y)
			return i;
	}
	return 0;
}

/* pathfinder_setup(setup_info)
 *
 * Allocate memory for grid-map and lookup-tables
 *
 * When done, setup_info must be passed to pathfinder_cleanup()
*/
int
pathfinder_setup(pathfinder_info *setup_info)
{
	/* Setup our binheap */
	setup_info->open_heap = bin_heap_new();

	/* Loopkup-maps for scores */
	setup_info->g_score = malloc(setup_info->grid_width * setup_info->grid_height);
	setup_info->h_score = malloc(setup_info->grid_width * setup_info->grid_height);

	/* Lookup-map for which list a node is
	   in (0 for none, 1 for open and 2 for closed )
	   (allows us to do this very fast, instead of
	   searching through our binary heaps)
	 */
	setup_info->node_type = malloc(setup_info->grid_width * setup_info->grid_height);

	setup_info->top_node = NULL;
	return 0;
}

/* Free memory,
   clean up
*/
void
pathfinder_cleanup(pathfinder_info *info)
{
	free(info->g_score);
	free(info->h_score);
	free(info->node_type);
	bin_heap_free(info->open_heap);
}

/*
 * pathfinder_free_nodes()
 *
 * Free all currently linked nodes
 * If reconstruct path has been called,
 * it has removed it's path from the linked list,
 * so this will not free the path
 */
static void
pathfinder_free_nodes(pathfinder_info *info)
{
	node_t *node;

	node = info->top_node;

	while (node) {
		node_t *tmp = node;
		node = node->next;
		free(tmp);
	}
	info->top_node = NULL;
}

/*
 * pathfinder_free_path(node)
 *
 * Free a path returned by pathfinder() (reconstruct_path())
 */
void
pathfinder_free_path(node_t *node)
{
	node_t *tmp_node;

	/* Go to top node */
	while (node->parent)
		node = node->parent;

	while (node) {
		tmp_node = node;
		node = node->child;
		free(tmp_node);
	}
}

/*
 * reconstruct_path(node)
 *
 * Walk upwards through node-list,
 * and create pointers to children.
 * Returns the first node.
 *
 * Also removes references to these nodes from binheap
 */
node_t *
reconstruct_path(pathfinder_info *info, node_t *node)
{
	node_t *child = NULL;
	node_t *tmp_node = NULL;

	for (;;) {
		node->child = child;
		child = node;

		/* Remove this node from node-list */
		tmp_node = node->next;
		if (node->next) node->next->prev = node->prev;
		if (node->prev) node->prev->next = tmp_node;
		if (info->top_node == node) info->top_node = node->next;

		/* Last node */
		if (!node->parent)
			return node;

		node = node->parent;
	}
	/* not reached */
	return NULL;
}

/*
 * pathfinder(info,monster, x1, y1, x2, y2)
 *
 * Get path between x1,y1 and x2,y2.
 * Uses info->is_walkable to determine if monster can
 * walk on grid position (x,y). Passes monster to this function
 */
node_t *
pathfinder(pathfinder_info *info, void *monster, int x1, int y1, int x2, int y2)// int *x2_ptr, int *y2_ptr)
{
	node_t *start_node;
	node_t *node;
	node_t *neighbour;
	int steps;

	unsigned char temp_g_score;

	start_node = node_new_pos(info, x1, y1);

	bin_heap_clear(info->open_heap);
	bin_heap_add(info->open_heap, start_node);

	/* Set all scores to 0 */
	memset(info->g_score, 0, info->grid_width*info->grid_height);
	memset(info->h_score, 0, info->grid_width*info->grid_height);

	/* Set all nodetypes to 0 */
	memset(info->node_type, 0, info->grid_width*info->grid_height);

	#define P(node) node->x + node->y*info->grid_width

	info->g_score[P(start_node)] = 0;
	info->h_score[P(start_node)] =
		manhattan(x1, y1, x2, y2);

	/* cost = f_score */
	start_node->cost = info->g_score[P(start_node)] + info->h_score[P(start_node)];

	steps = 0;

	while (info->open_heap->heap_size) {
		node = bin_heap_get_min(info->open_heap);
		steps ++;

		if (node->x == x2 &&
			node->y == y2) {

			/* Reverse path, and remove path
			 * from node-list
			 */
			node = reconstruct_path(info, node);

			/* clear up open-heap */
			bin_heap_clear(info->open_heap);
			pathfinder_free_nodes(info);
			return node;
		}

		bin_heap_del_min(info->open_heap);

		/* Set type to CLOSED */
		info->node_type[P(node)] = TYPE_CLOSED;

		int direction;

		/* Look at node's neighbours */
		for (direction=0;direction<4;direction++) {
			int x,y;

			x = node->x + mod_x[direction];
			y = node->y + mod_y[direction];

			/* Don't walk off map */
			if (x < 0 || y < 0 ||
				x > info->grid_width - 1 ||
				y > info->grid_height - 1)
				continue;

			/* Don't go back */
			if (node->parent &&
				 node->parent->x == x && node->parent->y == y)
				continue;

			/* check if we can walk that way */
			if (!info->is_walkable(x, y, monster))
				continue;

			temp_g_score = info->g_score[P(node)] + 1;

			char added = 0;
			char type;
			int neighbour_idx = 0;
			int curr_score;

			/* If neighbour in OPEN-set and cost is lower */
			type = info->node_type[x + y*info->grid_width];
			curr_score = info->g_score[x + y*info->grid_width];
			if (type != TYPE_CLOSED &&
				(curr_score == 0 ||
				temp_g_score < curr_score)) {

				/* Neighbour not in OPEN and not in CLOSED */
				if (type != TYPE_OPEN) {
					added = 1;
					neighbour = node_new_pos(info, x, y);
				}else{
					neighbour_idx = find_node(info->open_heap, x, y);
					neighbour = info->open_heap->data[neighbour_idx];
				}

				info->g_score[P(neighbour)] = temp_g_score;
				info->h_score[P(neighbour)] = manhattan(x, y, x2, y2);

				neighbour->cost =
					info->g_score[P(neighbour)] + info->h_score[P(neighbour)];
				neighbour->parent = node;

				/* We've just changed the cost for this node,
				   so update the binheap
				*/
				if (! added)
					bin_heap_bubble_up(info->open_heap, neighbour_idx);
				else {
					bin_heap_add(info->open_heap, neighbour);
					info->node_type[P(neighbour)] = TYPE_OPEN;
				}
			}
		}
	}

	pathfinder_free_nodes(info);
	return NULL;
}
