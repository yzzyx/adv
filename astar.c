#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include "map.h"
#include "common.h"
#include "gamestate.h"

#undef DEBUG
#define _log(...)

#define DIR_W 0
#define DIR_N 1
#define DIR_E 2
#define DIR_S 3

#define TYPE_OPEN 1
#define TYPE_CLOSED 2

#define JUMPLIMIT 100

int mod_x[4] = { -1,  0, 1, 0 };
int mod_y[4] = {  0, -1, 0, 1 };
char dir_name[4] = { 'W', 'N', 'E', 'S' };

char *type_name[3] = { "N/A", "OPEN", "CLOSED" };

int MAP_WIDTH;
int MAP_HEIGHT;

struct node;
typedef struct node{
	uint16_t x;
	uint16_t y;
	uint8_t cost;
	uint16_t steps;
	struct node *parent;
}node_t;

#define NODE_POOL_SIZE 100
struct node_pool{
	node_t *node;
	int allocated;
	int next_free;
};

struct node_pool *node_pools = NULL;
int node_pool_allocated = 0;
int node_pool_count = 0;

typedef struct{
	node_t **data;
	int heap_size;
	int array_size;
}bin_heap_t;

bin_heap_t *open_heap;
unsigned char *g_score, *h_score;
unsigned char *node_type;

node_t *node_new_pos(int x, int y)
{
	node_t *ptr;

	if( node_pool_allocated >  0 &&
		node_pools[node_pool_count].next_free >=
		node_pools[node_pool_count].allocated )
		node_pool_count ++;

	if( node_pool_allocated == 0 || 
		node_pool_count >= node_pool_allocated ){

		node_pool_allocated ++;
		node_pools = realloc(node_pools, node_pool_allocated * sizeof(struct node_pool));

		node_pools[node_pool_count].node =
			malloc(NODE_POOL_SIZE * sizeof(node_t));
		node_pools[node_pool_count].allocated = NODE_POOL_SIZE;
		node_pools[node_pool_count].next_free = 0;
	}

	ptr = &(node_pools[node_pool_count].node[
			node_pools[node_pool_count].next_free++]);

	ptr->x = x;
	ptr->y = y;
	ptr->cost = 0;
	ptr->parent = NULL;
	ptr->steps = 0;

	return ptr;
}

void node_pool_init()
{
	/* allocate node-pool here */
	if( node_pool_allocated >  0 &&
		node_pools[node_pool_count].next_free >=
		node_pools[node_pool_count].allocated )
		node_pool_count ++;

	if( node_pool_allocated == 0 || 
		node_pool_count >= node_pool_allocated ){

		node_pool_allocated ++;
		node_pools = realloc(node_pools, node_pool_allocated * sizeof(struct node_pool));

		node_pools[node_pool_count].node =
			malloc(NODE_POOL_SIZE * sizeof(node_t));
		node_pools[node_pool_count].allocated = NODE_POOL_SIZE;
		node_pools[node_pool_count].next_free = 0;
	}
}

/* When we don't want to use the node-pool anymore,
   we call this.
*/
void node_pool_cleanup()
{
	int i;

	for(i=0;i<node_pool_allocated;i++)
		free(node_pools[i].node);
	free(node_pools);

	node_pools = NULL;
	node_pool_allocated = 0;
	node_pool_count = 0;
}

void node_pool_free()
{
	int i;

	for(i=0;i<node_pool_allocated;i++){
		node_pools[i].next_free = 0;
	}
	node_pool_count = 0;
}

node_t *node_get_neighbour(node_t *node, char dir)
{
	if (dir == DIR_W) return node_new_pos(node->x-1,node->y);
	if (dir == DIR_E) return node_new_pos(node->x+1,node->y);
	if (dir == DIR_S) return node_new_pos(node->x,node->y+1);
	if (dir == DIR_N) return node_new_pos(node->x,node->y-1);
	return NULL;
}

inline int manhattan(node_t *n1, node_t *n2)
{
	int abs_x, abs_y;

	if (n1->x > n2->x) abs_x = n1->x - n2->x;
	else abs_x = n2->x - n1->x;

	if (n1->y > n2->y) abs_y = n1->y - n2->y;
	else abs_y = n2->y - n1->y;

	return abs_x + abs_y;
}

bin_heap_t *bin_heap_new()
{
	bin_heap_t *bh;

	bh = malloc(sizeof(bin_heap_t));

	/* FIXME! 200 ? varför? */
	bh->data = malloc(sizeof(node_t*)* 200);
	bh->array_size = 200;
	bh->heap_size = 0;

	return bh;
}

inline node_t *bin_heap_get_min(bin_heap_t *bh)
{
	if( bh->heap_size == 0 )
		return NULL;
	return bh->data[0];
}

void
bin_heap_clear(bin_heap_t *bh)
{
	bh->heap_size = 0;
}

void bin_heap_free(bin_heap_t *bh)
{
	free(bh->data);
	free(bh);
}

void bin_heap_free_data(bin_heap_t *bh)
{
	int i;
	for(i=0;i<bh->heap_size;i++)
		if( bh->data[i] ) free(bh->data[i]);
	bin_heap_free(bh);
}

#define bin_heap_left_child(bh,idx) 2*idx+1
#define bin_heap_right_child(bh,idx) 2*idx+2
#define bin_heap_parent(bh,idx) (idx-1)/2

void bin_heap_bubble_up(bin_heap_t *bh, int idx)
{
	int parent;
	node_t *tmp;
	while( idx != 0 ){

		parent = bin_heap_parent(bh,idx);
		if( bh->data[parent]->cost > bh->data[idx]->cost ){
			tmp = bh->data[parent];
			bh->data[parent] = bh->data[idx];
			bh->data[idx] = tmp;

			idx = parent;
		}else
			break;
	}
}

void bin_heap_bubble_down(bin_heap_t *bh)
{
	int left, right;
	int child;
	int idx;
	node_t *tmp;

	idx = 0;
	for(;;){
		left = bin_heap_left_child(bh,idx);
		right = bin_heap_right_child(bh,idx);

		/* We're at the bottom */
		if( left >= bh->heap_size && right >= bh->heap_size )
			return;

		/* One child */
		if( left >= bh->heap_size || right >= bh->heap_size ){
			child = (left > bh->heap_size) ? right : left;
		}else{
			/* Two children */
			if( bh->data[left]->cost < bh->data[right]->cost )
				child = left;
			else
				child = right;
		}

		if( bh->data[child]->cost < bh->data[idx]->cost ){
			tmp = bh->data[child];
			bh->data[child] = bh->data[idx];
			bh->data[idx] = tmp;
		}else
			return;

		idx = child;
	}
}

inline void bin_heap_add(bin_heap_t *bh, node_t *node)
{
	int idx;

	if( bh->heap_size == bh->array_size ){
		/* FIXME! - reallocate heap */
		printf("Binary heap overflow!");
		abort();
	}

	idx = bh->heap_size ++;
	bh->data[idx] = node;
	bin_heap_bubble_up(bh,idx);
}

inline void bin_heap_del_min(bin_heap_t *bh)
{
	bh->heap_size --;
	if( bh->heap_size == 0 ){
		return;
	}

	bh->data[0] = bh->data[bh->heap_size];
	bin_heap_bubble_down(bh);
}

char node_in_bin_heap(bin_heap_t *bh, node_t *node)
{
	int i;

	for(i=0;i<bh->heap_size;i++){
		if( bh->data[i]->x == node->x &&
			bh->data[i]->y == node->y )
			return 1;
	}
	return 0;
}

inline int
find_node(bin_heap_t *bh, int x, int y)
{
	int i;

	for(i=0;i<bh->heap_size;i++){
		if( bh->data[i]->x == x &&
			bh->data[i]->y == y )
			return i;
	}
	return 0;
}

/* Allocate memory and
   do some initialization
   stuff
*/
void
pathfinder_setup(int width, int height)
{
	/* save width and height for later */
	MAP_WIDTH = width;
	MAP_HEIGHT = height;

	/* Setup our binheap */
	open_heap = bin_heap_new();

	/* Loopkup-maps for scores */
	g_score = malloc(MAP_WIDTH*MAP_HEIGHT);
	h_score = malloc(MAP_WIDTH*MAP_HEIGHT);

	/* Lookup-map for which list a node is
	   in (0 for none, 1 for open and 2 for closed )
	   (allows us to do this very fast, instead of
	   searching through our binary heaps)
	 */
	node_type = malloc(MAP_WIDTH*MAP_HEIGHT);

	/* Setup node-pool */
	node_pool_init();
}

/* Free memory,
   clean up
*/
void
pathfinder_shutdown()
{
	free(g_score);
	free(h_score);
	free(node_type);
	bin_heap_free(open_heap);
	node_pool_cleanup();
}

node_t *
find_jump_node(adv_monster *m, int dir,
		node_t *node, node_t *end_node)
{
	int steps;
	int x, y;
	node_t jump_node;
	node_t *n;

	x = node->x;
	y = node->y;
	jump_node.x = x;
	jump_node.y = y;

	switch (dir) {
		/*
		...%%
		....!  <- här är en jump-point (5)
		....!  <- här är en jump-point (2)
		...%
		....!  <- här är en jump-point (1)
		....!  <- här är en jump-point (2) + (4)
		%%%%
		...%
		....!  <- här är en jump-point (1) + (3)
		....^
		....A  <- start

		*/
		case DIR_N:
			for (steps = 1; steps <= JUMPLIMIT; steps++) {
				int ny = node->y - steps;

				jump_node.y = ny;

				if (ny == 0)
					break;

				/* If we pass over the end-node's y-coordinate,
				   make a jump-point
				 */
				if (ny == end_node->y)
					break;

				/* JP 5 */
				if (!map_is_walkable(m,x,ny-1))
					break;

				/* JP 1 */
				if (x > 0 &&
				     map_is_walkable(m, x-1, ny) &&
				    !map_is_walkable(m, x-1, ny-1))
					break;

				/* JP 2 */
				if (x > 0 &&
				   !map_is_walkable(m, x-1, ny+1) &&
				    map_is_walkable(m, x-1, ny))
					break;

				/* JP 3 */
				if (x < MAP_WIDTH - 1 &&
				     map_is_walkable(m, x+1, ny) &&
				    !map_is_walkable(m, x+1, ny-1))
					break;

				/* JP 4 */
				if (x < MAP_WIDTH - 1 &&
				   !map_is_walkable(m, x+1, ny+1) &&
				    map_is_walkable(m, x+1, ny))
					break;
			}
		break;
		/*
		....A  <- start
		....v
		....!  <- här är en jump-point (1) + (3)
		...%
		%%%%
		....!  <- här är en jump-point (2) + (4)
		....!  <- här är en jump-point (1)
		...%
		....!  <- här är en jump-point (2)
		....   <- här är en jump-point (5)
		...%%
		*/
		case DIR_S:
			for (steps = 1; steps <= JUMPLIMIT; steps++) {
				int ny = node->y + steps;

				jump_node.y = ny;

				if (ny == global_GS.current_map->height - 1)
					break;

				/* If we pass over the end-node's y-coordinate,
				   make a jump-point
				 */
				if (ny == end_node->y)
					break;

				/* JP 5 */
				if (!map_is_walkable(m, x, ny+1))
					break;

				/* JP 1 */
				if (x > 0 &&
				     map_is_walkable(m, x-1, ny) &&
				    !map_is_walkable(m, x-1, ny+1))
					break;

				/* JP 2 */
				if (x > 0 &&
				   !map_is_walkable(m, x-1, ny-1) &&
				    map_is_walkable(m, x-1, ny))
					break;

				/* JP 3 */
				if (x < MAP_WIDTH - 1 &&
				     map_is_walkable(m, x+1, ny) &&
				    !map_is_walkable(m, x+1, ny+1))
					break;

				/* JP 4 */
				if (x < MAP_WIDTH - 1 &&
				   !map_is_walkable(m, x+1, ny-1) &&
				    map_is_walkable(m, x+1, ny))
					break;
			}
		break;
		/*
		+------------- Start
		|  +---------- jump-point (1) + (3)
		|  |  +------- jump-point (2) + (4)
		|  |  |+------ jump-point (1) + (3)
		|  |  ||
		|  |  || +---- jump-point (2) + (4)
		|  |  || |+--- jump-point (5)
		v  v  vv vv
		A->!  !! !!%
		....%%..%..%
		.....%.....%
		.....%......
		*/
		case DIR_E:
			for (steps = 1; steps <= JUMPLIMIT; steps++) {
				int nx = node->x + steps;

				jump_node.x = nx;

				if (nx == global_GS.current_map->width - 1)
					break;

				/* If we pass over the end-node's x-coordinate,
				   make a jump-point
				 */
				if (nx == end_node->x)
					break;

				/* JP 5 */
				if (!map_is_walkable(m, nx+1, y))
					break;

				/* JP 1 */
				if (y > 0 &&
				     map_is_walkable(m, nx  , y-1) &&
				    !map_is_walkable(m, nx+1, y-1))
					break;

				/* JP 2 */
				if (y > 0 &&
				   !map_is_walkable(m, nx-1, y-1) &&
				    map_is_walkable(m, nx  , y-1))
					break;

				/* JP 3 */
				if (y < MAP_HEIGHT - 1 &&
				     map_is_walkable(m, nx  , y+1) &&
				    !map_is_walkable(m, nx+1, y+1))
					break;

				/* JP 4 */
				if (y < MAP_HEIGHT - 1 &&
				   !map_is_walkable(m, nx-1, y+1) &&
				    map_is_walkable(m, nx  , y+1))
					break;
			}
		break;
		/*
		 +----------- jump-point (5)
		 |+---------- jump-point (2) + (4)
		 || +-------- jump-point (1) + (3)
		 || |+------- jump-point (2) + (4)
		 || ||
		 || ||  +---- jump-point (1) + (3)
		 || ||  | +-- start point
		 vv vv  v  v
		%!! !!  !<-A
		%..%..%%....
		%.....%.....
		......%.....
		*/
		case DIR_W:
			for (steps = 1; steps <= JUMPLIMIT; steps++) {
				int nx = node->x - steps;

				jump_node.x = nx;

				if (nx == 0)
					break;

				/* If we pass over the end-node's x-coordinate,
				   make a jump-point
				 */
				if (nx == end_node->x)
					break;

				/* JP 5 */
				if (!map_is_walkable(m, nx-1, y))
					break;

				/* JP 1 */
				if (y > 0 &&
				     map_is_walkable(m, nx  , y-1) &&
				    !map_is_walkable(m, nx-1, y-1))
					break;

				/* JP 2 */
				if (y > 0 &&
				   !map_is_walkable(m, nx+1, y-1) &&
				    map_is_walkable(m, nx  , y-1))
					break;

				/* JP 3 */
				if (y < MAP_HEIGHT - 1 &&
				     map_is_walkable(m, nx  , y+1) &&
				    !map_is_walkable(m, nx-1, y+1))
					break;

				/* JP 4 */
				if (y < MAP_HEIGHT - 1 &&
				    map_is_walkable(m, nx+1, y+1) &&
				   !map_is_walkable(m, nx  , y+1))
					break;
			}
		break;
	}

	if (jump_node.x < 0 || jump_node.x >= global_GS.current_map->width ||
		jump_node.y < 0 || jump_node.y >= global_GS.current_map->height)
		return NULL;

	/* Allocate a new node, and return it */
	n = node_new_pos(jump_node.x, jump_node.y);
	n->steps = steps;
	return n;
}

int
get_first_direction(node_t *node)
{
	char dir;

	if (node->parent == NULL)
		return -1;

	/* Go to first node */
	while (node->parent && node->parent->parent) {
		node = node->parent;
	}

	if (node->parent->x > node->x) { dir = DIRECTION_LEFT; }
	else if (node->parent->x < node->x) { dir = DIRECTION_RIGHT; }
	else if (node->parent->y > node->y) { dir = DIRECTION_UP; }
	else if (node->parent->y < node->y) { dir = DIRECTION_DOWN; }
	else { dir = -1; }
	
	return dir;
}
char *
reconstruct_path(node_t *node)
{
	node_t *end_node;
	char *path;
	char dir;
	int steps = 0;
	int mx, my;
	int nx, ny;

	end_node = node;

	/* Count number of steps */
	while (node->parent) {
		mx = 0; my = 0;
		if (node->parent->x > node->x) { mx = 1; }
		else if (node->parent->x < node->x) { mx = -1; }
		else if (node->parent->y > node->y) { my = 1; }
		else if (node->parent->y < node->y) { my = -1; }

		for(nx=node->x, ny = node->y;
				nx != node->parent->x ||
				ny != node->parent->y;
				nx += mx, ny += my) {
			steps++;
		}
		node = node->parent;
	}

	path = malloc((steps+1)*sizeof(char));
	path[steps--] = '\0';
	node = end_node;

	/* Print path */
	while (node->parent) {
		mx = 0; my = 0;
		if (node->parent->x > node->x) { mx = 1; dir = 'W'; }
		else if (node->parent->x < node->x) { mx = -1; dir = 'E'; }
		else if (node->parent->y > node->y) { my = 1; dir = 'N'; }
		else if (node->parent->y < node->y) { my = -1; dir = 'S'; }

		for(nx=node->x, ny = node->y;
				nx != node->parent->x ||
				ny != node->parent->y;
				nx += mx, ny += my) {
			path[steps--] = dir;
		}
		node = node->parent;
	}

	_log("Path '%s'\n", path);
	return path;
}

int
pathfinder(adv_monster *m, int x1, int y1, int *x2_ptr, int *y2_ptr)
{
	node_t *start_node, *end_node;
	node_t *node;
	node_t *neighbour;
	int steps;
	int x2, y2;

	x2 = *x2_ptr;
	y2 = *y2_ptr;

	unsigned char temp_g_score;

	_log("Path from %d,%d -> %d,%d\n", x1, y1, x2, y2);
	/* If we can't go to target, check neighbours */
	if (!map_is_walkable(m, x2, y2)) {
		printf("Cannot walk to %d,%d\n", x2, y2);
		int xmod = 0, ymod = 0;
		int t; int i;
		int dx = 0, dy = -1;

		//int maxI = (8 * r * (r-1)) / 2;

		for (i = 0; i < 8;i ++) {

			if ((xmod == ymod) ||
			    ((xmod < 0) && (xmod == -ymod)) ||
			    ((xmod > 0) && (xmod == 1-ymod))) {
				    t = dx;
				    dx = -dy;
				    dy = t;
			}
			xmod += dx;
			ymod += dy;

			if (map_is_walkable(m, x2+xmod, y2+ymod)) {
				/* FIXME - do whole lap, then (if it exists)
				 * pick the closest
				 */
				*x2_ptr = x2+xmod;
				*y2_ptr = y2+ymod;
				return pathfinder(m, x1, y1, x2_ptr, y2_ptr);
			}
		}
	}

//	print_map(map, MAP_WIDTH,MAP_HEIGHT);

	start_node = node_new_pos(x1, y1);
	end_node = node_new_pos(x2, y2);

	bin_heap_clear(open_heap);
	bin_heap_add(open_heap, start_node);

	/* Set all scores to 0 */
	memset(g_score, 0, MAP_WIDTH*MAP_HEIGHT);
	memset(h_score, 0, MAP_WIDTH*MAP_HEIGHT);

	/* Set all nodes t0 0 */
	memset(node_type, 0, MAP_WIDTH*MAP_HEIGHT);

	#define P(node) node->x + node->y*MAP_WIDTH

	g_score[P(start_node)] = 0;
	h_score[P(start_node)] =
		manhattan(start_node, end_node);

	/* cost = f_score */
	start_node->cost = g_score[P(start_node)] + h_score[P(start_node)];

	steps = 0;

	while (open_heap->heap_size) {
		node = bin_heap_get_min(open_heap);
		steps ++;

		if (node->x == end_node->x &&
			node->y == end_node->y) {

			return get_first_direction(node);
#ifdef DEBUG
			int mx, my;
			int nx, ny;
			while (node->parent) {
				mx = 0; my = 0;
				if (node->parent->x > node->x) mx = 1;
				else if (node->parent->x < node->x) mx = -1;
				else if (node->parent->y > node->y) my = 1;
				else if (node->parent->y < node->y) my = -1;

				for(nx=node->x, ny = node->y;
					nx != node->parent->x ||
					ny != node->parent->y;
					nx += mx, ny += my) {
				}
				node = node->parent;

			}
#endif
			break;

		}

		bin_heap_del_min(open_heap);

		/* Set type to CLOSED */
		node_type[P(node)] = TYPE_CLOSED;

		int direction;

		/* Look at node's neighbours */
		for (direction=0;direction<4;direction++) {
			int x,y;

			/* Don't walk off map */
			if ((direction == DIR_W && node->x == 0) ||
				(direction == DIR_N && node->y == 0) ||
				(direction == DIR_E && node->x == MAP_WIDTH-1) ||
				(direction == DIR_S && node->y == MAP_HEIGHT-1))
				continue;

			x = node->x + mod_x[direction];
			y = node->y + mod_y[direction];

			/* Skip walls */
//			if(map->tiles[x + y*map->width]->walkable == 0){
			if (!map_is_walkable(m,x,y)) {
				continue;
			}

			/* Don't go back */
			if (node->parent &&
				 ((node->parent->x < node->x && direction == DIR_W) ||
				  (node->parent->x > node->x && direction == DIR_E) ||
				  (node->parent->y < node->y && direction == DIR_N) ||
				  (node->parent->y > node->y && direction == DIR_S)))
				continue;

			/* Find jump-points */
			neighbour = find_jump_node(m, direction,  node, end_node);

			if (! neighbour)
				continue;

			if (neighbour->x < 0 || neighbour->x > MAP_WIDTH - 1 ||
				neighbour->y < 0 || neighbour->y > MAP_HEIGHT - 1)
				continue;

			temp_g_score = g_score[P(node)] + neighbour->steps;
			_log("(%.2d,%.2d) %c node %.2d,%.2d : g-score: %d (was %d) type: %s ",
					node->x, node->y,
					dir_name[direction],
					neighbour->x, neighbour->y,
					temp_g_score, g_score[P(neighbour)],
					type_name[node_type[P(neighbour)]]);

			char added = 0;
			char type;
			int neighbour_idx = 0;
			node_t *neighbour_real = NULL;

			if ((type = node_type[P(neighbour)]) != TYPE_CLOSED &&
				(g_score[P(neighbour)] == 0 ||
				temp_g_score < g_score[P(neighbour)])) {

				/* Update node */
				if (type != TYPE_OPEN) {
					added = 1;
					neighbour_real = neighbour;
					/* hmmm... */
				//node_new_pos(x,y);
				}else{
					neighbour_idx = find_node(open_heap,
							neighbour->x, neighbour->y);
					neighbour_real = open_heap->data[neighbour_idx];
				}

				neighbour_real->parent = node;
				g_score[P(neighbour)] = temp_g_score;
				h_score[P(neighbour)] = manhattan(neighbour, end_node);

				neighbour_real->cost = 
					g_score[P(neighbour)] + h_score[P(neighbour)];

				/* We've just changed the cost for this node,
				   so update the binheap
				*/
				if (! added)
					bin_heap_bubble_up(open_heap, neighbour_idx);
				else {
					bin_heap_add(open_heap, neighbour);
					node_type[P(neighbour)] = TYPE_OPEN;
				}
			}


			neighbour_idx = find_node(open_heap, neighbour->x, neighbour->y);
			if (neighbour_real) {
				_log(" cost: %d  -> index in heap: %d/%d\n", neighbour_real->cost,
				    neighbour_idx, open_heap->heap_size);
			}

		}
	}
	return ' ';
}
