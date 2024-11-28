/*==========================================================*
 *
 * @author Gustaf Franzén :: https://github.com/BjorneEk;
 *
 * sketch graph
 *
 *==========================================================*/

#include "sketch_graph.h"
#include "line.h"

#include <string.h>
#include <stdio.h>

void graph_set(sketch_graph_t *g, v3f_t a, v3f_t b)
{
	g->data[IMAP_get(g->vert_idx, &a, sizeof(v3f_t)) + g->width * IMAP_get(g->vert_idx, &b, sizeof(v3f_t))] = true;
}

void graph_unset(sketch_graph_t *g, v3f_t a, v3f_t b)
{
	g->data[IMAP_get(g->vert_idx, &a, sizeof(v3f_t)) + g->width * IMAP_get(g->vert_idx, &b, sizeof(v3f_t))] = false;
}

bool graph_get(sketch_graph_t *g, v3f_t a, v3f_t b)
{
	return g->data[IMAP_get(g->vert_idx, &a, sizeof(v3f_t)) + g->width * IMAP_get(g->vert_idx, &b, sizeof(v3f_t))];
}

sketch_graph_t *generate_graph(dla_t *lines)
{
	sketch_graph_t *res;

	res = malloc(sizeof(sketch_graph_t));
	res->vert_idx = IMAP_new(lines->len * 1.5, HASH_fnv_1a);
	res->vertecies = malloc(lines->len*2*sizeof(v3f_t));

	res->width = 0;
	FOREACH_DLA(lines, i, sketch_line_t, l, {
		if (!IMAP_contins_key(res->vert_idx, &l.line.p1, sizeof(v3f_t))) {
			memcpy(&res->vertecies[res->width], &l.line.p1, sizeof(v3f_t));
			IMAP_add(res->vert_idx, &res->vertecies[res->width], sizeof(v3f_t), res->width);
			//printf("ADDED1: %.1f, %.1f, %.1f : %llu \n", l.p1.x, l.p1.y, l.p1.z, IMAP_get(res->vert_idx, &res->vertecies[res->width], sizeof(v3f_t)));

			++res->width;
		}
		if (!IMAP_contins_key(res->vert_idx, &l.line.p2, sizeof(v3f_t))) {
			memcpy(&res->vertecies[res->width], &l.line.p2, sizeof(v3f_t));
			IMAP_add(res->vert_idx, &res->vertecies[res->width], sizeof(v3f_t), res->width);
			//printf("ADDED2: %.1f, %.1f, %.1f : %llu \n", l.p2.x, l.p2.y, l.p2.z, IMAP_get(res->vert_idx, &res->vertecies[res->width], sizeof(v3f_t)));

			++res->width;
		}
	});

	res->data = calloc(res->width * res->width, sizeof(bool));

	FOREACH_DLA(lines, i, sketch_line_t, l, {
		graph_set(res, l.line.p1, l.line.p2);
		graph_set(res, l.line.p2, l.line.p1);
	});

	return res;
}

void free_sketch_graph(sketch_graph_t **g)
{
	IMAP_free(&(*g)->vert_idx);
	free((*g)->vertecies);
	free((*g)->data);
	free(*g);
}

void print_sketch_graph(sketch_graph_t *g)
{
	printf("Vertecies\n");
	u64_t i, j;

	for (i = 0; i < g->width; i++) {
		V3_print(g->vertecies[i]);
		printf(" : %llu\n", IMAP_get(g->vert_idx, &g->vertecies[i], sizeof(v3f_t)));
	}
	printf("Graph\n	");
	for (i = 0; i < g->width; i++) {
		printf("%llu	", i);
	}
	printf("\n");
	for (i = 0; i < g->width; i++) {
		printf("%llu	", i);
		for (j = 0; j < g->width; j++) {
			printf("%i	", g->data[j + i * g->width]);
		}
		printf("\n");
	}
}

