/*==========================================================*
 *
 * @author Gustaf Franzén :: https://github.com/BjorneEk;
 *
 * object
 *
 *==========================================================*/

#include "object.h"
#include "../matrix/matrix.h"
#include "../matrix/vector.h"
#include "../GL/font.h"
#include "../structures/dynamic_array.h"
#include "../sketch/segment.h"
#include <malloc/_malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <string.h>

static v3f_t object_COLOR = {0.7, 0.7, 0.7};

face_t *new_face(v3f_t pos, v3f_t *verts, u64_t vert_cnt, u32_t *mesh, u64_t mesh_len, line_t *border, u64_t border_len)
{
	face_t *res;

	if (mesh_len < 3) {
		fprintf(stderr, "Cannot create face from %llu faces\n", mesh_len);
		exit(-1);
	}

	res = malloc(sizeof(face_t));
	res->border = border;
	res->border_len = border_len;
	res->mesh = mesh;
	res->mesh_len = mesh_len;
	res->verts = verts;
	res->vert_count = vert_cnt;
	res->normal = V3_normalized(V3_normal(verts[mesh[0]], verts[mesh[1]], verts[mesh[2]]));
	res->pos = pos;
	glGenVertexArrays(1, &res->VAO);
	glGenBuffers(1, &res->VBO);
	glGenBuffers(1, &res->EBO);

	glBindVertexArray(res->VAO);

	glBindBuffer(GL_ARRAY_BUFFER, res->VBO);
	glBufferData(GL_ARRAY_BUFFER, res->vert_count * 3 * sizeof(f32_t), res->verts, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, res->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, res->mesh_len * sizeof(u32_t), res->mesh, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(f32_t), (void*)0);
	glEnableVertexAttribArray(0);

	return res;
}
void free_face(face_t **f)
{
	free((*f)->border);
	free((*f)->verts);
	free((*f)->mesh);
	free(*f);
}

bool face_contains(face_t *f, v3f_t p)
{
	u32_t i;

	for (i = 0; i < f->mesh_len; i += 3) {
		v3f_t tri[3] = {
			f->verts[f->mesh[i]],
			f->verts[f->mesh[i + 1]],
			f->verts[f->mesh[i + 2]]
		};
		if (triangle_contains(tri, p))
			return true;
	}
	return false;
}

object_t *new_object(face_t *faces, u64_t face_count, selectable_t *parent)
{
	u32_t mesh_len;
	u32_t i, j;
	u32_t mesh_idx;
	object_t *res;

	res = malloc(sizeof(object_t));

	res->face_count = face_count;
	res->faces = faces;

	mesh_len = 0;
	for (i = 0; i <face_count; i++)
		mesh_len += faces[i].mesh_len;

	res->mesh = malloc(3 * mesh_len * sizeof(v3f_t));
	mesh_idx = 0;
	for (i = 0; i < face_count; i++) {
		for (j = 0; j < faces[i].mesh_len; j++) {
			res->mesh[mesh_idx * 3] = V3_add(faces[i].verts[faces[i].mesh[j]], faces[i].pos);
			res->mesh[mesh_idx * 3 + 1] = faces[i].normal;
			res->mesh[mesh_idx * 3 + 1] = V3(0.0, 0.0, 0.0);
			++mesh_idx;
		}
	}
	res->mesh_length = mesh_len;
	res->parent = parent;
	return res;

}

void draw_border_line(line_t l, shader_t shader, v4f_t color, f32_t *view, f32_t *proj)
{
	u32_t VAO, VBO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(f32_t), &l, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	shader_use(&shader);
	shader_setV4(&shader, "color", color);
	shader_setM4(&shader, "projection", proj);
	shader_setM4(&shader, "view", view);

	//glClear(GL_DEPTH_BUFFER_BIT);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	glDisable(GL_CULL_FACE);

	glDisable(GL_BLEND);
	//glDisable(GL_CULL_FACE);


	glBindVertexArray(VAO);
	glDrawArrays(GL_LINES, 0, 6);
	glBindVertexArray(0);
}
line_t line_add(line_t l, v3f_t p)
{
	return line(V3_add(l.p1, p), V3_add(l.p2, p));
}
void render_face_border(face_t *f, shader_t shader, v4f_t color, f32_t *view, f32_t *proj, bool show_normal)
{
	u64_t i;
	u32_t VAO;
	u32_t VBO;
	for(i = 0; i < f->border_len; i++)
		draw_border_line(line_add(f->border[i], f->pos), shader, color, view, proj);
	if (show_normal) {
		draw_border_line(line(V3_add(f->pos, f->verts[f->mesh[0]]), V3_add(V3_add(f->pos, f->verts[f->mesh[0]]), V3_scale(f->normal, 0.05))), shader, color, view, proj);
	}

}

void render_face(face_t *f, shader_t shader, v4f_t color, f32_t *rot, f32_t *view, f32_t *proj, bool cull_face)
{
	shader_use(&shader);
	shader_setV4(&shader, "color", color);
	shader_setM4(&shader, "projection", proj);
	shader_setM4(&shader, "view", view);
	shader_setM4(&shader, "rotation", rot);
	shader_setV3(&shader, "normal", f->normal);
	shader_setV3(&shader, "pos", f->pos);

	//glClear(GL_DEPTH_BUFFER_BIT);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	if (!cull_face)
		glDisable(GL_CULL_FACE);

	glBindVertexArray(f->VAO);
	glDrawElements(GL_TRIANGLES, f->mesh_len, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void render_object_(object_t *o, shader_t shader, shader_t border_shader, v3f_t color, v4f_t border_color, f32_t *rot, f32_t *view, f32_t *proj, bool show_edges)
{
	u32_t i, j;
	for (i = 0; i < o->mesh_length; i++)
		o->mesh[i * 3 + 2] = color;

}

void render_object(object_t *o, shader_t shader,shader_t border_shader, v4f_t color, v4f_t border_color, f32_t *rot, f32_t *view, f32_t *proj, bool show_edges)
{
	u32_t i;
	for (i = 0; i < o->face_count; i++) {
		render_face(&o->faces[i], shader, color, rot, view, proj, true);
		if (show_edges)
			render_face_border(&o->faces[i], border_shader, border_color, view, proj, true);
	}
}
