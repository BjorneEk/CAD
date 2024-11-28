/*==========================================================*
 *
 * @author Gustaf Franzén :: https://github.com/BjorneEk;
 *
 * sketch graph
 *
 *==========================================================*/

#ifndef _SKETCH_GRAPH_H_
#define _SKETCH_GRAPH_H_

#include "../matrix/matrix.h"
#include "../matrix/vector.h"
#include "../GL/shader.h"
#include "../structures/dynamic_array.h"
#include "../structures/intmap.h"
#include "../selectable/selectable.h"

typedef struct sketch_graph {

	u32_t width;

	v3f_t *vertecies;

	intmap_t *vert_idx;

	bool *data;
} sketch_graph_t;

#define FOREACH_NEIGHBOR(_G, _from, _idx, __stmt__)						\
do {												\
	u64_t _idx;										\
	for (_idx = (_from) * (_G)->width; _idx < ((_from) + 1) * (_G)->width; _idx++) {	\
		if ((_G)->data[_idx] && _idx - ((_from) * (_G)->width) != _from) {		\
			_idx -= ((_from) * (_G)->width);					\
			__stmt__								\
			_idx += ((_from) * (_G)->width);					\
		}										\
	}											\
} while(0);



sketch_graph_t *generate_graph(dla_t *lines);
void print_sketch_graph(sketch_graph_t *g);
void free_sketch_graph(sketch_graph_t **g);

void	find_cycles(sketch_graph_t *g, u64_t start_idx, dla_t *res);

dla_t *find_all_cycles(sketch_graph_t *g);


dla_t *get_cycles(dla_t *lines, selectable_t parent);

#endif /* _SKETCH_GRAPH_H_ */