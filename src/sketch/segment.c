/*==========================================================*
 *
 * @author Gustaf Franzén :: https://github.com/BjorneEk;
 *
 * complete cycle sketch representation
 *
 *==========================================================*/
#include "segment.h"
#include "sketch.h"
#include "sketch_graph.h"
#include "../GL/font.h"
#include "../GL/draw.h"
#include "../matrix/matrix.h"
#include "../matrix/vector.h"
#include "../GL/font.h"
#include "../structures/dynamic_array.h"
#include "line.h"
#include <stdlib.h>
#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <string.h>

static const f32_t EPSILON = 0.0000000001f;
static const f32_t SELECT_DIST = 0.01;
static v3f_t segment_SELECTED_LINE_COLOR = {0.0, 0.0, 1.0};
static v3f_t segment_LINE_COLOR = {1.0, 0.6, 1.0};

static v3f_t segment_SELECTED_CYCLE_LINE_COLOR = {0.0, 0.0, 1.0};
static v3f_t segment_CYCLE_LINE_COLOR = {1.0, 1.0, 0.6};

void triangulate_cycle(cycle_t *cycle);

bool cycle_contains(cycle_t c, v3f_t p)
{
	u32_t i;

	for (i = 0; i < c.mesh_len; i += 3) {
		v3f_t tri[3] = {
			c.verts[c.mesh[i]],
			c.verts[c.mesh[i + 1]],
			c.verts[c.mesh[i + 2]]
		};
		if (triangle_contains(tri, p))
			return true;
	}
	return false;
}

bool segment_try_select(segment_t *seg, v3f_t real_pos, segment_t **res)
{
	u32_t i;
	if (seg->type == CYCLE && cycle_contains(seg->cycle, real_pos)) {
		*res = seg;
		return true;
	} else if (seg->type == UNCLOSED) {
		for (i = 0; i < seg->length; i++) {
			if (dist_to_line(seg->lines[i], real_pos) < SELECT_DIST) {
				*res = seg;
				return true;
			}
		}
	}
	return false;
}


void show_segment(segment_t *seg, shader_t shader, font_t *font, f32_t *view, f32_t *proj, bool selected, bool hovered)
{
	u64_t j;
	v3f_t clr;
	shader_use(&shader);
	if (seg->type == UNCLOSED) {
		clr = selected ? segment_SELECTED_LINE_COLOR : segment_LINE_COLOR;
		clr = hovered ? V3_scale(clr, 0.9) : clr;

		for (j = 0; j < seg->length; j++)
				draw_line(seg->lines[j], shader, font, clr, view, proj);
		return;
	}
	clr = selected ? segment_SELECTED_CYCLE_LINE_COLOR : segment_CYCLE_LINE_COLOR;
	for(j = 0; j < seg->length; j++)
		draw_line(seg->lines[j], shader, font, clr, view, proj);

	if (!hovered && !selected)
		return;

	shader_setV4(&shader, "color", V3_to_V4(clr, 0.3));
	shader_setM4(&shader, "projection", proj);
	shader_setM4(&shader, "view", view);

	glClear(GL_DEPTH_BUFFER_BIT);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	glDisable(GL_CULL_FACE);

	glBindVertexArray(seg->cycle.VAO);
	glDrawElements(GL_TRIANGLES, seg->cycle.mesh_len, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
dla_t *finish_sketch(sketch_t **sketch)
{
	dla_t *new_segments;
	dla_t *result;

	new_segments = get_cycles((*sketch)->lines, (*sketch)->parent);
	result = DLA_new(sizeof(selectable_t), new_segments->len);

	FOREACH_DLA(new_segments, i, selectable_t, s, {
		if (s.type == CYCLE)
			triangulate_cycle(&s.cycle->cycle);
		DLA_append(result, &s);
	});
	//printf("here\n");
	DLA_free(&(*sketch)->lines);
	free(*sketch);
	return result;
}
// void finish_sketch(sketch_t **sketch, dla_t *segments)
// {
// 	dla_t *new_segments;
// 	new_segments = get_cycles((*sketch)->lines, (*sketch)->parent);
// 	FOREACH_DLA(new_segments, i, selectable_t, s, {
// 		if (s.type == CYCLE)
// 			triangulate_cycle(&s.cycle->cycle);
// 		DLA_append(segments, &s);
// 	});
// 	//printf("here\n");
// 	DLA_free(&(*sketch)->lines);
// 	free(*sketch);
// }

f64_t poly_area(v2f_t *verts, u64_t *c, u64_t len)
{
	i32_t p, q;
	f64_t a;

	a = 0.0;

	for(p = len - 1, q = 0; q < len; p = q++)
		a +=verts[c[p]].x * verts[c[q]].y - verts[c[q]].x * verts[c[p]].y;
	return a * 0.5;
}
bool in_tri(v2f_t a, v2f_t b, v2f_t c, v2f_t p)

{
	float ax, ay, bx, by, cx, cy, apx, apy, bpx, bpy, cpx, cpy;
	float cCROSSap, bCROSScp, aCROSSbp;

	ax = c.x - b.x;	ay = c.y - b.y;
	bx = a.x - c.x;	by = a.y - c.y;
	cx = b.x - a.x;	cy = b.y - a.y;
	apx= p.x - a.x;	apy= p.y - a.y;
	bpx= p.x - b.x;	bpy= p.y - b.y;
	cpx= p.x - c.x;	cpy= p.y - c.y;

	aCROSSbp = ax*bpy - ay*bpx;
	cCROSSap = cx*apy - cy*apx;
	bCROSScp = bx*cpy - by*cpx;

	return ((aCROSSbp >= 0.0f) && (bCROSScp >= 0.0f) && (cCROSSap >= 0.0f));
};
bool poly_snip(v2f_t *verts, u64_t *poly, u64_t len, u64_t a, u64_t b, u64_t c, u64_t n)
{
	u64_t p;

	if ( EPSILON > (
		((verts[poly[b]].x-verts[poly[a]].x)*(verts[poly[c]].y-verts[poly[a]].y)) -
		((verts[poly[b]].y-verts[poly[a]].y)*(verts[poly[c]].x-verts[poly[a]].x))))
		return false;

	for (p = 0; p < n; p++) {
		if( (p == a) || (p == b) || (p == c) )
			continue;

		v3f_t tri[3] = {
			V2_to_V3(verts[poly[a]], 0.0),
			V2_to_V3(verts[poly[b]], 0.0),
			V2_to_V3(verts[poly[c]], 0.0),
		};

		if (in_tri(verts[poly[a]], verts[poly[b]], verts[poly[c]], verts[poly[p]]))
			return false;
	}
	return true;
}

u32_t *triangulate(v2f_t *verts, u64_t *poly, u64_t len, u64_t *res_len)
{
	u32_t *res;
	u64_t res_size;
	u64_t count;
	u64_t nv;
	u64_t m, v, u, w;
	u64_t a,b,c,s,t;

	if (len < 3) {
		return NULL;
		res_len = 0;
	}

	if (0.0f >= poly_area(verts, poly, len))
		reverse(poly, len);

	res = malloc((len-2)*3*sizeof(u32_t));

	nv = len;
	count = 2 * nv;
	res_size = 0;

	for(m = 0, v = nv - 1; nv > 2; ) {
		if (0 >= (count--)) {
			//printf("Error\n");
			free(res);
			return NULL;
		}

		u = v;
		if (nv <= u)
			u = 0;
		v = u + 1;
		if (nv <= v)
			v = 0;
		w = v + 1;
		if (nv <= w)
			w = 0;

		if (poly_snip(verts, poly, len, u, v, w, nv)) {

			a = poly[u]; b = poly[v]; c = poly[w];

			res[res_size++] = (u32_t)poly[u];
			res[res_size++] = (u32_t)poly[v];
			res[res_size++] = (u32_t)poly[w];

			m++;

			for(s = v, t = v + 1; t < nv; s++, t++) {
				poly[s] = poly[t];
			}
			nv--;

			count = 2 * nv;
		}
	}
	*res_len = res_size;
	return res;
}

void gen_globject(cycle_t *cycle)
{
	glGenVertexArrays(1, &cycle->VAO);
	glGenBuffers(1, &cycle->VBO);
	glGenBuffers(1, &cycle->EBO);

	glBindVertexArray(cycle->VAO);

	glBindBuffer(GL_ARRAY_BUFFER, cycle->VBO);
	glBufferData(GL_ARRAY_BUFFER, cycle->length * 3 * sizeof(f32_t), cycle->verts, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cycle->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, cycle->mesh_len * sizeof(u32_t), cycle->mesh, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(f32_t), (void*)0);
	glEnableVertexAttribArray(0);


}
void triangulate_cycle(cycle_t *cycle)
{
	f32_t *convert_2d;
	v2f_t *verts;
	u64_t *poly;
	u64_t *triangulated;
	u64_t vert_count;
	u64_t tmp;
	v3f_t v_;
	u64_t i;
	u64_t atempts;

	atempts = 0;

	convert_2d = rotational_transform(V3_normalized(V3_cross(
		V3_sub(cycle->verts[cycle->cycle[1]], cycle->verts[cycle->cycle[0]]),
		V3_sub(cycle->verts[cycle->cycle[2]], cycle->verts[cycle->cycle[0]]))),V3(0.0, 0.0, 1.0));

	verts = malloc(cycle->length * sizeof(v2f_t));
	poly = malloc(cycle->length * sizeof(u64_t));

	memcpy(poly, cycle->cycle, cycle->length * sizeof(u64_t));

	for (i = 0; i < cycle->length; i++) {
		v_ = M4_mult_V3(convert_2d, cycle->verts[i]);
		verts[i] = V2(v_.x, v_.y);
	}
	free(convert_2d);
retry:
	cycle->mesh = triangulate(verts, poly, cycle->length, &vert_count);
	if (!cycle->mesh && atempts < cycle->length) {
		++atempts;
		memcpy(poly, cycle->cycle + atempts, (cycle->length - atempts) * sizeof(u64_t));
		memcpy(poly + cycle->length - atempts, cycle->cycle, atempts * sizeof(u64_t));
		reverse(poly + cycle->length - atempts, atempts);
		goto retry;
	} else if (!cycle->mesh) {
		fprintf(stderr, "Failed triangulation polygon");
		exit(-1);
	}
	free(verts);
	free(poly);
	cycle->mesh_len = vert_count;
	gen_globject(cycle);
}
