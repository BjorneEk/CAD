/*==========================================================*
 *
 * @author Gustaf Franzén :: https://github.com/BjorneEk;
 *
 * sketch representation
 *
 *==========================================================*/
#include "../object/object.h"
#include "../plane.h"

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


static const float EPSILON = 0.0000000001f;

static v3f_t sketch_LINE_COLOR = {0.0, 0.0, 0.0};
static v3f_t sketch_SELECTED_LINE_COLOR = {0.9, 0.7, 0.0};


sketch_t *new_sketch(selectable_t parent)
{
	sketch_t *res;
	if (parent.type != FACE && parent.type != PLANE) {
		fprintf(stderr, "failed to create plane\n");
		return NULL;
	}
	res = malloc(sizeof(sketch_t));
	res->lines = DLA_new(sizeof(sketch_line_t), 10);
	res->parent = parent;
	res->has_p1 = false;
	return res;
}

v3f_t sketch_normal(sketch_t *sketch)
{
	if (sketch->parent.type == FACE)
		return sketch->parent.face->normal;
	if (sketch->parent.type == PLANE)
		return sketch->parent.plane->normal;
}


void draw_line(line_t line, shader_t shader, font_t *font, v3f_t color, f32_t *view, f32_t *proj)
{
	char str[10];
	v3f_t mid, diff;
	f32_t *view_;
	f32_t *rot;

	u32_t VAO;
	u32_t VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(f32_t), &line, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	shader_use(&shader);
	shader_setV4(&shader, "color", V4(color.x, color.y, color.z, 1.0));
	shader_setM4(&shader, "projection", proj);
	shader_setM4(&shader, "view", view);

	glClear(GL_DEPTH_BUFFER_BIT);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	glDisable(GL_CULL_FACE);

	glDisable(GL_BLEND);
	//glDisable(GL_CULL_FACE);


	glBindVertexArray(VAO);
	glDrawArrays(GL_LINES, 0, 6);
	glBindVertexArray(0);
	//glDeleteVertexArrays(1, &VAO);
	//glDeleteBuffers(1, &VBO);

	// sprintf(str, "%f", V3_len(V3_sub(line.p2, line.p1)));

	// view_ = M4_copy(view);
	// mid = V3_sub(line.p2, line.p1);

	// rot = M4_rotation(M4_mult_V3(view, V3(0.0, 0.0, 1.0)), line.p1.y < line.p2.y ? acos(mid.x/V3_len(mid)) : -acos(mid.x/V3_len(mid)));

	// diff = V3_scale(V3_normalized(mid), (font_str_width(font, str, 0.0005)) * 0.5);
	// mid = V3_sub(V3_add(V3_scale(mid, 0.5), line.p1), diff);

	// M4_mult(rot, view_);

	// //M4_scale(&rot, 0.0005);
	// render_text_(font, mid.x, mid.y, 0.0005, V3(0.0, 0.0, 0.0), rot, proj, str);
	// free(rot);
	// free(view_);

	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);
}




void show_sketch(sketch_t *sketch, shader_t shader, font_t *font, f32_t *view, f32_t *proj, v3f_t p2)
{

	if (sketch->has_p1) {
		draw_line(line(sketch->p1, p2), shader, font, sketch_SELECTED_LINE_COLOR, view, proj);
	}
	FOREACH_DLA(sketch->lines, i, sketch_line_t, l, {
		//draw_line(l, shader, font, V3((float)i/sketch->lines->len, 0.0, 0.0), view, proj);
		if (l.selected)
			draw_line(l.line, shader, font, sketch_SELECTED_LINE_COLOR, view, proj);
		else
			draw_line(l.line, shader, font, sketch_LINE_COLOR, view, proj);
		//draw_dot_normal(l.p1, V3(0.0, 0.0, 1.0), 0.01, V4(1.0,1.0,1.0,1.0), view, proj);
		//draw_dot_normal(l.p2, V3(0.0, 0.0, 1.0), 0.01, V4(1.0,1.0,1.0,1.0), view, proj);
	})


	// u32_t VAO;

	// VAO = V3_VAO((f32_t*)sketch->lines->data, sketch->lines->len * 6);

	// shader_use(&shader);
	// shader_setV4(&shader, "color", sketch_LINE_COLOR);
	// shader_setM4(&shader, "projection", proj);
	// shader_setM4(&shader, "view", view);

	// glClear(GL_DEPTH_BUFFER_BIT);
	// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// glEnable(GL_BLEND);

	// glDisable(GL_CULL_FACE);

	// glBindVertexArray(VAO);
	// glDrawArrays(GL_LINES, 0, sketch->lines->len * 6);
	// glBindVertexArray(0);

	// glEnable(GL_CULL_FACE);
	// glDisable(GL_BLEND);
}

void try_snap_sketch(sketch_t *sketch, v3f_t *pos, f32_t snap_dist)
{
	f32_t dist;
	v3f_t proj;

	if (sketch->lines->len <= 0)
		return;

	FOREACH_DLA(sketch->lines, i, sketch_line_t, l, {
		dist = V3_len(V3_sub(l.line.p1, * pos));
		if (dist <= snap_dist) {
			*pos = l.line.p1;
			return;
		}
		dist = V3_len(V3_sub(l.line.p2, * pos));
		if (dist <= snap_dist) {
			*pos = l.line.p2;
			return;
		}
		proj = V3_proj_point_line(l.line.p1, l.line.p2, *pos);
		if (is_on_line(l.line, proj) && V3_len(V3_sub(proj, * pos)) <= snap_dist) {
			*pos = proj;
			return;
		}
	});
}

void deselect_all(sketch_t *sketch)
{
	FOREACH_DLA_PTR(sketch->lines, i, sketch_line_t, l, {
		l->selected = false;
	});
}
void sketch_select(sketch_t *sketch, v3f_t pos, f32_t select_dist)
{
	bool none_selected;

	none_selected = true;
	FOREACH_DLA_PTR(sketch->lines, i, sketch_line_t, l, {
		if (dist_to_line(l->line, pos) < select_dist) {
			l->selected = !l->selected;
			none_selected = false;
		}
	});
	if (none_selected) {
		deselect_all(sketch);
	}
}
void delete_selected(sketch_t *sketch)
{
	dla_t *new_lines;

	new_lines = DLA_new(sizeof(sketch_line_t), sketch->lines->len);
	FOREACH_DLA_PTR(sketch->lines, i, sketch_line_t, l, {
		if (!l->selected)
			DLA_append(new_lines, l);
	});
	DLA_free(&sketch->lines);
	sketch->lines = new_lines;
}
void sketch_add(sketch_t *sketch, v3f_t p)
{
	if (sketch->has_p1) {
		add_line(sketch, line(sketch->p1, p));
		sketch->p1 = p;
		return;
	}
	sketch->has_p1 = true;
	sketch->p1 = p;
}

void add_line(sketch_t *sketch, line_t new_line)
{
	sketch_line_t new;
	if(v3_safe_eq(new_line.p1, new_line.p2))
		return;

	FOREACH_DLA_PTR(sketch->lines, i, sketch_line_t, l, {
		if(v3_safe_eq(new_line.p1, l->line.p1)) {
			new_line.p1 = l->line.p1;
		} else if(v3_safe_eq(new_line.p1, l->line.p2)) {
			new_line.p1 = l->line.p2;
		} if(v3_safe_eq(new_line.p2, l->line.p1)) {
			new_line.p2 = l->line.p1;
		} else if(v3_safe_eq(new_line.p2, l->line.p2)) {
			new_line.p2 = l->line.p2;
		}
		if (line_contains(l->line, new_line.p1)) {
			//printf("split p1\n");
			new.line = line(new_line.p1, l->line.p2);
			new.selected = false;
			l->line.p2 = new_line.p1;
			DLA_append(sketch->lines, &new);
		}
		if (line_contains(l->line, new_line.p2)) {
			//printf("split p2\n");
			new.line = line(new_line.p2, l->line.p2);
			new.selected = false;
			l->line.p2 = new_line.p2;
			DLA_append(sketch->lines, &new);
		}

	});
	new.line = new_line;
	new.selected = false;
	DLA_append(sketch->lines, &new_line);
}

