/*==========================================================*
 *
 * @author Gustaf Franzén :: https://github.com/BjorneEk;
 *
 * plane representation
 *
 *==========================================================*/

#include "line.h"
#include "segment.h"
#include "sketch_graph.h"
#include "matrix/vector.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>



typedef struct node {
	u64_t val;
	struct node *next;
} node_t;

node_t *new_node(u64_t vertex)
{
	node_t *res = malloc(sizeof(node_t));
	res->val = vertex;
	res->next = NULL;
	return res;
}

void push(node_t **n, u64_t val)
{
	node_t *head;
	node_t *new;

	if (*n == NULL) { /* empty stack */
		*n = new_node(val);
		(*n)->next = *n;
		return;
	}

	if ((*n)->next == *n) { /* single element stack */
		head = *n;
		new = new_node(val);
		head->next = new;
		new->next = head;
		*n = new;
		return;
	}

	head = (*n)->next;
	new = new_node(val);
	new->next = head;
	(*n)->next = new;
	*n = new;
}

u64_t pop(node_t **n)
{
	u64_t val;
	node_t *new_head;

	if (*n == NULL) { /* empty stack */
		fprintf(stderr, "Tried popping empty stack, %s :%i\n", __FILE__, __LINE__);
		exit(-1);
	}

	if ((*n)->next == *n) { /* single element stack */
		val = (*n)->val;
		free(*n);
		*n = NULL;
		return val;
	}

	val = (*n)->next->val;
	new_head = (*n)->next->next;
	free((*n)->next);
	(*n)->next = new_head;
	return val;
}

typedef struct idx_cycle {
	u64_t *indicies;
	u64_t width;
} idx_cycle_t;

u64_t get_idx(sketch_graph_t *g, v3f_t v)
{
	return IMAP_get(g->vert_idx, &v, sizeof(v3f_t));
}

node_t *find_path(bool *data, u64_t width, u64_t start_idx, u64_t end_idx, bool *found, u64_t *cycle_len)
{
	node_t *stack = NULL;
	node_t *path = NULL;
	u64_t vert_idx;
	i32_t *level;
	u64_t *prev;
	u64_t *res;

	u64_t i;
	u64_t idx;

	u64_t prev_val;

	//printf("searching from %llu to %llu\n", start_idx, end_idx);
	push(&stack, start_idx);
	level = malloc(width * sizeof(i32_t));
	prev = calloc(width, sizeof(u64_t));
	memset(level, -1, width * sizeof(i32_t));
	*found = false;
	level[start_idx] = 0;

	while (stack != NULL) {
		vert_idx = pop(&stack);
		for (i = vert_idx * width; i < (vert_idx + 1) * width; i++) {
			idx = i - vert_idx * width;
			if (data[i] && idx != vert_idx) {
				if (level[idx] == -1) {
					level[idx] = level[vert_idx] + 1;
					prev[idx] = vert_idx;
					push(&stack, idx);
				}
				if (idx == end_idx) {
					*found = true;
					res = malloc(level[idx]*sizeof(u64_t));
					prev_val = idx;
					*cycle_len = level[idx];
					i = 0;
					do {
						push(&path, prev_val);
						//printf("prev: %llu\n", prev_val);
						prev_val = prev[prev_val];
					} while (prev_val != start_idx);
					goto found_cycle;
				}
			}
		}
	}
found_cycle:
	if (*found)
		return path;
	else
		return NULL;
}

int cmp_u64(const void *a, const void *b)
{
	return *((u64_t*)a) - *((u64_t*)b);
}

void shift_lowes_cycle_elem_first(u64_t *cycle, u64_t len)
{
	u64_t i;
	u64_t lowest_idx;
	u64_t *tmp_buff;
	u64_t tmp;

	lowest_idx = 0;

	if (len <= 1)
		return;

	for(i = 0; i < len; i ++)
		if (cycle[lowest_idx] > cycle[i])
			lowest_idx = i;

	if (lowest_idx == 0)
		return;

	if (lowest_idx == len - 1) {
		tmp = cycle[lowest_idx];
		memcpy(cycle + 1, cycle, (len - 1) * sizeof(u64_t));
		cycle[0] = tmp;
		return;
	}

	tmp_buff = malloc((len - lowest_idx) * sizeof(u64_t));
	memcpy(tmp_buff, cycle + lowest_idx, (len - lowest_idx) * sizeof(u64_t));
	memcpy(cycle + (len - lowest_idx), cycle, lowest_idx * sizeof(u64_t));
	memcpy(cycle, tmp_buff, (len - lowest_idx) * sizeof(u64_t));
	free(tmp_buff);
}

void reverse(u64_t *arr, u64_t len)
{
	u64_t i;
	u64_t tmp;

	for(i = 0; i < len / 2; i++) {
		tmp = arr[i];
		arr[i] = arr[len-1-i];
		arr[len-1-i] = tmp;
	}
}

/**
 * transforms the cycles so that they start at their lowest indicies,
 * they are equal if they are equal or the reverse of one of the cycles is equal to the other
 *
 * permanently transforms the cycles;
 **/
bool cycle_equals(idx_cycle_t a, idx_cycle_t b)
{
	if (a.width != b.width)
		return false;

	shift_lowes_cycle_elem_first(a.indicies, a.width);
	shift_lowes_cycle_elem_first(b.indicies, b.width);
	if (!memcmp(a.indicies, b.indicies, b.width * sizeof(u64_t)))
		return true;
	reverse(b.indicies + 1, b.width - 1);
	if (!memcmp(a.indicies, b.indicies, b.width * sizeof(u64_t)))
		return true;
	return false;
}
void add_cycle(dla_t *cycles, u64_t *cycle, u64_t cycle_len)
{
	idx_cycle_t res;
	bool duplicate;

	res.indicies = cycle;
	res.width = cycle_len;

	duplicate = false;
	FOREACH_DLA(cycles, j, idx_cycle_t, c, {
		if(cycle_equals(res, c))
			duplicate = true;
	});
	if (!duplicate)
		DLA_append(cycles, &res);
}

void add_cycle_stack(dla_t *cycles, node_t *cycle, u64_t cycle_len)
{
	u64_t *indicies;
	u64_t i;
	bool duplicate;

	indicies = malloc(cycle_len *sizeof(u64_t));
	i = 0;

	while(cycle != NULL)
		indicies[i++] = pop(&cycle);

	add_cycle(cycles, indicies, cycle_len);
}

void	find_cycles(sketch_graph_t *g, u64_t start_idx, dla_t *res)
{
	node_t *stack = NULL;
	u64_t cycle_len;
	u64_t i;
	u64_t idx;
	bool found;

	for (i = start_idx * g->width; i < (start_idx + 1) * g->width; i++) {
		idx = i - start_idx * g->width;
		if (g->data[i] && idx != start_idx) {
			g->data[i] = false;
			g->data[idx * g->width + start_idx] = false;
			stack = find_path(g->data, g->width, idx, start_idx, &found, &cycle_len);
			if (found) {
				push(&stack, idx);
				add_cycle_stack(res, stack, cycle_len + 1);
			}
			g->data[i] = true;
			g->data[idx * g->width + start_idx] = true;
		}
	}
	return;
}

dla_t *find_all_cycles(sketch_graph_t *g)
{
	dla_t *res;
	dla_t *cycles;
	u64_t i;

	res = DLA_new(sizeof(idx_cycle_t), 10);

	for (i = 0; i < g->width; i++)
		find_cycles(g, i, res);

	return res;
}
void print_line(line_t l)
{
	printf("%f, %f, %f -> %f, %f, %f", l.p1.x, l.p1.y, l.p1.z, l.p2.x, l.p2.y, l.p2.z);
}
bool contains_edge(idx_cycle_t c, u32_t i1, u32_t i2)
{
	u32_t i;

	if (	c.indicies[0] == i1 && c.indicies[c.width - 1] == i2 ||
		c.indicies[0] == i2 && c.indicies[c.width - 1] == i1)
		return true;

	for (i = 0; i < c.width - 1; i++) {
		if(	c.indicies[i] == i1 && c.indicies[i + 1] == i2 ||
			c.indicies[i] == i2 && c.indicies[i + 1] == i1)
			return true;
	}
	return false;
}
bool seg_contains(dla_t *segments, u32_t i)
{
	FOREACH_DLA(segments, j, idx_cycle_t, c, {
		for (i = 0; i < c.width - 1; i++) {
			if (c.indicies[i] == i)
				return true;
		}
	});
	return false;
}

void find_unclosed_rec(sketch_graph_t *g, dla_t *lines, dla_t *cycles, dla_t *segments, u32_t prev, u32_t idx, node_t **res, bool first)
{
	u32_t i;
	u32_t next;
	line_t l;
	push(res, idx);
	if (!first) {
		l = line(g->vertecies[prev], g->vertecies[idx]);
		DLA_append(lines, &l);
	}
	for (i = idx * g->width; i < (idx + 1) * g->width; i++) {
		next = i - idx * g->width;
		if (g->data[i] && idx != next && (first || prev != next)) {
			l = line(g->vertecies[prev], g->vertecies[idx]);
			DLA_append(lines, &l);
			push(res, next);
			if (!seg_contains(cycles, next) && !seg_contains(segments, next))
				find_unclosed_rec(g, lines, cycles, segments, idx, next, res, false);
		}
	}
}
idx_cycle_t gen_cycle(node_t **n, u32_t len)
{
	idx_cycle_t res;
	u32_t i;
	res.indicies = malloc(len*sizeof(u64_t));
	res.width = len;
	i = 0;
	while ((*n) != NULL)
		res.indicies[i] = pop(n);
	return res;
}
void find_unclosed_segments(sketch_graph_t *g, dla_t *cycles, dla_t *segments)
{
	u32_t i;
	dla_t *lines;
	node_t *res = NULL;
	segment_t *seg;
	idx_cycle_t new_seg_cycle;
	dla_t *seg_cycles;

	seg_cycles = DLA_new(sizeof(idx_cycle_t), 5);
	lines = DLA_new(sizeof(line_t), 5);
	for (i = 0; i < g->width; i++) {

		find_unclosed_rec(g, lines, cycles, seg_cycles, 0, i, &res, true);

		if (lines->len == 0) {
			continue;
			printf("here\n");
		}
		new_seg_cycle = gen_cycle(&res, lines->len);

		DLA_append(seg_cycles, &new_seg_cycle);
		seg = malloc(sizeof(segment_t));
		seg->type = UNCLOSED;
		seg->length = lines->len;
		seg->lines = malloc(lines->len * sizeof (line_t));
		memcpy(seg->lines, lines, lines->len * sizeof(line_t));
		DLA_clear(lines);
		DLA_append(segments, &seg);
	}
	DLA_free(&lines);
	FOREACH_DLA(seg_cycles, i, idx_cycle_t, c, {
		free(c.indicies);
	});
	DLA_free(&seg_cycles);

}

dla_t *get_cycles(dla_t *lines, selectable_t parent)
{
	sketch_graph_t *g;
	dla_t *result; /*segment_t*/
	segment_t *cycle;
	selectable_t s;
	dla_t *cycles; /* idx_cycle_t */
	v3f_t prev;
	v3f_t next;
	u64_t i;



	g = generate_graph(lines);
	//print_sketch_graph(g);
	cycles = find_all_cycles(g);

	result = DLA_new(sizeof(selectable_t), 10);
	FOREACH_DLA(cycles, j_, idx_cycle_t, c, {
		cycle = malloc(sizeof(segment_t));
		cycle->parent = parent;
		cycle->type = CYCLE;
		cycle->lines = malloc(c.width * sizeof (line_t));
		cycle->cycle.verts = malloc(c.width * sizeof(v3f_t));
		cycle->cycle.cycle = malloc(c.width * sizeof(u64_t));
		cycle->length = c.width;
		cycle->cycle.length = c.width;
		for(i = 0; i < c.width; i++) {
			cycle->cycle.cycle[i] = i;
			cycle->cycle.verts[i] = g->vertecies[c.indicies[i]];
		}

		prev = g->vertecies[c.indicies[0]];
		cycle->lines[c.width-1] = line(g->vertecies[c.indicies[c.width - 1]], prev);
		//printf("cycle:\n");
		//for (j = 0; j < c.width-1; j++) {
		//	printf("%llu ->", c.indicies[j]);
		//}
		//printf("%llu\n", c.indicies[c.width-1]);
		for (i = 1; i < c.width; i++) {
			next = g->vertecies[c.indicies[i]];
			cycle->lines[i-1] = line(prev, next);
			prev = next;
			//print_line(cycle.lines[j-1]);
			//printf("\n");
		}
		//print_line(cycle.lines[c.width-1]);
		//printf("\n");
		free(c.indicies);
		s = selectable(CYCLE, cycle, NULL);
		DLA_append(result, &s);
	})
	//find_unclosed_segments(g, cycles, result);
	// FOREACH_DLA(cycles, i, idx_cycle_t, c, {
	// 	free(c.indicies);
	// });
	// FOREACH_DLA(result, i, segment_t*, s, {
	// 	if (s->type == CYCLE)
	// 		for(int k = 0; k < s->length; k++) {
	// 			printf("%llu, ", s->cycle.cycle[k]);
	// 		}
	// 	else {
	// 		for(int k = 0; k < s->length; k++) {
	// 			V3_print(s->lines[k].p1);
	// 			printf("	-> ");
	// 			V3_print(s->lines[k].p2);
	// 			printf("\n");
	// 		}
	// 	}
	// 	printf("\n");
	// })
	DLA_free(&cycles);
	free_sketch_graph(&g);
	return result;
}


