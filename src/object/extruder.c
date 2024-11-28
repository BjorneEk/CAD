/*==========================================================*
 *
 * @author Gustaf Franzén :: https://github.com/BjorneEk;
 *
 * extrusions
 *==========================================================*/

#include "extruder.h"
#include "../sketch/segment.h"
#include "object.h"
#include <stdio.h>
#include <string.h>

static v3f_t extruder_COLOR = {0.0, 0.0, 1.0};
extruder_t *new_extruder()
{
	extruder_t *res;

	res = malloc(sizeof(extruder_t));

	res->has_parent = false;
	return res;
}
void invert_winding(u32_t *mesh, u64_t mesh_len)
{
	u64_t i;
	u32_t tmp;
	for (i = 0; i < mesh_len; i += 3) {
		tmp = mesh[i];
		mesh[i] = mesh[i + 1];
		mesh[i + 1] = tmp;
	}
}

void extruder_update(extruder_t *ex, v3f_t move_vec)
{
	face_t *tmp;
	v3f_t new;
	new = V3_add(ex->top->pos, move_vec);

	//move_amount = V3_dot(move_vec, ex->normal);
	extruder_set_dist(ex, V3_len(new));
}
void extruder_set_dist(extruder_t *ex, f32_t dist)
{
	face_t *tmp;
	if (ex->dist >= 0 && dist < 0 || ex->dist < 0 && dist >= 0) {
		ex->invert_wind = !ex->invert_wind;
		tmp = ex->bottom;
		ex->bottom = ex->top;
		ex->bottom->pos = V3(0.0, 0.0, 0.0);
		ex->top = tmp;
	}
	ex->dist = dist;

	ex->top->pos = V3_scale(ex->normal, dist);
}
f32_t cross2(v3f_t a, v3f_t b)
{
	return (a.x * b.y) - (a.y * b.x);
}
bool rotation_inverted(line_t *l, u32_t len, v3f_t norm)
{
	f32_t *inv;
	u32_t i;
	i32_t min;
	v3f_t a, b, c;
	v3f_t new;
	f32_t res;

	inv = rotational_transform(norm, V3(0.0, 0.0, 1.0));
	min = -1;
	a = M4_mult_V3(inv, l[0].p1);
	for(i = 0; i < len; i++) {
		new = l[i].p2;
		//new = M4_mult_V3(inv, l[i].p2);
		if (new.y < a.y || (new.y == a.y && new.x > a.x)) {
			a = new;
			min = i;
		}
	}
	if (min == 0) {
		b = l[0].p1;
		c = l[1].p2;
	} else if (min == len - 1) {
		b = l[min].p1;
		c = l[0].p2;
	} else if (min == -1) {
		b = l[len - 1].p1;
		c = l[0].p2;
	} else {
		b = l[min - 1].p2;
		c = l[min + 1].p2;
	}
	free(inv);
	return (cross2(V3_sub(b, a), V3_sub(c, a)) < 0);
}
void	set_extrude_geometry(extruder_t *e, selectable_t *s)
{
	v3f_t *verts;
	u32_t *mesh;
	line_t *border;
	side_face_t *sides;
	u32_t i;
	if (e->has_parent) {
		e->parent = NULL;
		free_face(&(e->bottom));
		free_face(&(e->top));
		free(e->sides);
	}
	e->has_parent = true;
	e->parent = s;

	verts = malloc(s->cycle->cycle.length * sizeof(v3f_t));
	mesh = malloc(s->cycle->cycle.mesh_len * sizeof(u32_t));
	border = malloc(s->cycle->cycle.length * sizeof(line_t));

	memcpy(verts,	s->cycle->cycle.verts,	s->cycle->cycle.length * sizeof(v3f_t));
	memcpy(mesh,	s->cycle->cycle.mesh,	s->cycle->cycle.mesh_len * sizeof(u32_t));
	memcpy(border,	s->cycle->lines,	s->cycle->cycle.length * sizeof(line_t));

	e->normal = V3_scale(V3_normalized(V3_normal(verts[mesh[0]], verts[mesh[1]], verts[mesh[2]])), -1.0f);
	e->has_parent = true;
	e->bottom = new_face(V3(0.0, 0.0, 0.0), verts, s->cycle->cycle.length, mesh, s->cycle->cycle.mesh_len, border, s->cycle->cycle.length);

	verts = malloc(s->cycle->cycle.length * sizeof(v3f_t));
	mesh = malloc(s->cycle->cycle.mesh_len * sizeof(u32_t));
	border = malloc(s->cycle->cycle.length * sizeof(line_t));

	memcpy(verts,	s->cycle->cycle.verts,	s->cycle->cycle.length * sizeof(v3f_t));
	memcpy(mesh,	s->cycle->cycle.mesh,	s->cycle->cycle.mesh_len * sizeof(u32_t));
	memcpy(border,	s->cycle->lines,	s->cycle->cycle.length * sizeof(line_t));

	invert_winding(mesh, s->cycle->cycle.mesh_len);



	e->top = new_face(V3(0.0, 0.0, 0.0), verts, s->cycle->cycle.length, mesh, s->cycle->cycle.mesh_len, border, s->cycle->cycle.length);

	sides = malloc(s->cycle->cycle.length * sizeof(side_face_t));
	i = 0;
	for(i = 0; i < s->cycle->cycle.length; i++) {
		sides[i].top[0] = e->top->border[i].p1;
		sides[i].top[1] = e->top->border[i].p2;
		sides[i].bottom[0] = e->bottom->border[i].p1;
		sides[i].bottom[1] = e->bottom->border[i].p2;

	}
	e->invert_wind = rotation_inverted(s->cycle->lines, s->cycle->cycle.length, e->normal);
	printf("inverted: %i\n", e->invert_wind);
	e->sides = sides;
	e->side_count = s->cycle->cycle.length;

	// printf("bottom ptr: %p, to: %p\n", e->bottom, (void*)(((u64_t)e->bottom) + sizeof(face_t)));
	// printf("top ptr: %p, to: %p\n", e->top, (void*)(((u64_t)e->top) + sizeof(face_t)));
	// printf("sides ptr: %p, to: %p\n", e->sides, &e->sides[e->side_count-1]);
	extruder_set_dist(e, 0.5);

}

void render_side_face_border(extruder_t *ex, side_face_t f, shader_t shader, v4f_t color, f32_t *view, f32_t *proj, bool inv_wind)
{
	GL_object_t obj;
	v3f_t normal;
	v3f_t top_[2] = {
		V3_add(f.top[0], ex->top->pos),
		V3_add(f.top[1], ex->top->pos),
	};
	v3f_t bottom_[2] = {
		V3_add(f.bottom[0], ex->bottom->pos),
		V3_add(f.bottom[1], ex->bottom->pos),
	};
	normal = inv_wind ?	V3_normalized(V3_normal(top_[1], top_[0], bottom_[0])) :
					V3_normalized(V3_normal(top_[0], top_[1], bottom_[0]));
	shader_use(&shader);
	shader_setV4(&shader, "color", color);
	shader_setM4(&shader, "projection", proj);
	shader_setM4(&shader, "view", view);
	shader_setV3(&shader, "pos", V3(0.0, 0.0, 0.0));

	v3f_t border[] = {
		top_[1], top_[0], top_[0], bottom_[0],
		bottom_[0], bottom_[1], bottom_[1], top_[1]
	};

	obj = V3_VAO((f32_t*)border, 8*3);
	//glClear(GL_DEPTH_BUFFER_BIT);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	glDisable(GL_CULL_FACE);
	glBindVertexArray(obj.VAO);
	glDrawArrays(GL_LINES, 0, obj.length);
	glBindVertexArray(0);
	//draw_border_line(line(top_[0], V3_add(top_[0], V3_scale(normal, 0.05))), shader, color, view, proj);
}
const u32_t side_mesh[] = {
	1, 0, 2, 1, 2, 3
};
const u32_t side_mesh_inverted[] = {
	0, 1, 2, 2, 1, 3
};
void face_from_side(face_t *res, extruder_t *ex, side_face_t s)
{
	v3f_t *verts;
	//v3f_t normal;
	u32_t *mesh;
	line_t *border;
	u32_t tmp;

	verts = malloc(4 * sizeof(v3f_t));

	verts[0] = V3_add(s.top[0], ex->top->pos);
	verts[1] = V3_add(s.top[1], ex->top->pos);
	verts[2] = V3_add(s.bottom[0], ex->bottom->pos);
	verts[3] = V3_add(s.bottom[1], ex->bottom->pos);

	//printf("verts from side: \n");
	// V3_print(verts[0]);
	// printf("\n");
	// V3_print(verts[1]);
	// printf("\n");
	// V3_print(verts[2]);
	// printf("\n");
	// V3_print(verts[3]);
	// printf("\n\n");

	//normal = ex->invert_wind ?	V3_normalized(V3_normal(verts[1], verts[0], verts[2])) :
	//				V3_normalized(V3_normal(verts[0], verts[1], verts[2]));


	mesh = malloc(6 * sizeof(u32_t));
	mesh[0] = 1;
	mesh[1] = 0;
	mesh[2] = 2;
	mesh[3] = 1;
	mesh[4] = 2;
	mesh[5] = 3;
	if (ex->invert_wind) {
		tmp = mesh[0];
		mesh[0] = mesh[1];
		mesh[1] = tmp;
		tmp = mesh[3];
		mesh[3] = mesh[4];
		mesh[4] = tmp;
	}
	border = malloc(4 * sizeof(line_t));

	// printf("creating face\n");
	// printf("verts ptr: %p, to: %p\n", verts, &verts[3]);
	// printf("mesh ptr: %p, to: %p\n", mesh, &mesh[5]);
	// printf("border ptr: %p, to: %p\n", border, &border[3]);
	border[0] = line(verts[0], verts[1]);
	border[1] = line(verts[2], verts[3]);

	border[2] = line(verts[1], verts[3]);
	border[3] = line(verts[0], verts[2]);

	res->border = border;
	res->border_len = 4;
	res->mesh = mesh;
	res->mesh_len = 6;
	res->verts = verts;
	res->vert_count = 4;

	res->normal = V3_normalized(V3_normal(verts[mesh[0]], verts[mesh[1]], verts[mesh[2]]));
	res->pos = V3(0.0f, 0.0f, 0.0f);

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
	return;
	//return new_face(V3(0.0, 0.0, 0.0), verts, 4, mesh, 8, border, 4);


}

object_t *finish_extrusion(extruder_t *ex)
{
	object_t *res;
	face_t *faces;
	u32_t face_count;
	u32_t i;


	face_count = ex->side_count + 2;
	//res->face_count = 2;
	faces = malloc(face_count * sizeof(face_t));
	//printf("obj ptr: %p, to :%p", res, (void*)(((u64_t)res) + sizeof(object_t)));
	//printf("faces ptr: %p, to :%p", res->faces, &res->faces[res->face_count-1]);
	//printf("side count: %u\n", ex->side_count);
	for (i = 0; i < ex->side_count; i++) {
		//memcpy(&res->faces[i], ex->bottom, sizeof(face_t));
		face_from_side(&faces[i], ex, ex->sides[i]);
	}



	memcpy(&faces[face_count -1], ex->bottom, sizeof(face_t));
	memcpy(&faces[face_count - 2], ex->top, sizeof(face_t));
	//memcpy(&res->faces[0], ex->bottom, sizeof(face_t));
	//memcpy(&res->faces[1], ex->top, sizeof(face_t));
	res = new_object(faces, face_count, ex->parent);
	return res;
}
void free_extruder(extruder_t **ex, bool has_extruded)
{
	if (!(*ex)->has_parent) {
		free(*ex);
		return;
	}
	if (has_extruded) {
		free((*ex)->bottom);
		free((*ex)->top);
		free((*ex)->sides);
		free(*ex);
		return;
	}
	free_face(&(*ex)->bottom);
	free_face(&(*ex)->top);
	free((*ex)->sides);
	free(*ex);
	return;
}

void render_side_face(extruder_t *ex, side_face_t f, shader_t shader, v4f_t color, f32_t *rot, f32_t *view, f32_t *proj, bool inv_wind)
{
	GL_object_t obj;
	v3f_t normal;
	v3f_t tmp;
	v3f_t top_[2] = {
		V3_add(f.top[0], ex->top->pos),
		V3_add(f.top[1], ex->top->pos),
	};
	v3f_t bottom_[2] = {
		V3_add(f.bottom[0], ex->bottom->pos),
		V3_add(f.bottom[1], ex->bottom->pos),
	};
	normal = inv_wind ?	V3_normalized(V3_normal(top_[1], top_[0], bottom_[0])) :
					V3_normalized(V3_normal(top_[0], top_[1], bottom_[0]));

	shader_use(&shader);
	shader_setV4(&shader, "color", color);
	shader_setM4(&shader, "projection", proj);
	shader_setM4(&shader, "view", view);
	shader_setM4(&shader, "rotation", rot);
	shader_setV3(&shader, "normal", normal);
	shader_setV3(&shader, "pos", V3(0.0, 0.0, 0.0));

	v3f_t mesh[6] = {
		top_[1], top_[0], bottom_[0],
		top_[1], bottom_[0], bottom_[1]
	};
	if (inv_wind) {
		tmp = mesh[0];
		mesh[0] = mesh[1];
		mesh[1] = tmp;
		tmp = mesh[3];
		mesh[3] = mesh[4];
		mesh[4] = tmp;
	}
	obj = V3_VAO((f32_t*)mesh, 18);
	//glClear(GL_DEPTH_BUFFER_BIT);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	glDisable(GL_CULL_FACE);
	glBindVertexArray(obj.VAO);
	glDrawArrays(GL_TRIANGLES, 0, obj.length);
	glBindVertexArray(0);
}

v3f_t	extruder_project(extruder_t *e, v3f_t inv_mouse, v3f_t look_dir)
{
	return V3_raycast_plane(e->top->normal, e->top->pos, inv_mouse, look_dir);
}

void render_extruder(extruder_t *ex, shader_t face_shader, shader_t border_shader, f32_t *rot, f32_t *view, f32_t *proj, bool show_border)
{
	u32_t i;
	if (!ex->has_parent)
		return;
	glClear(GL_DEPTH_BUFFER_BIT);
	render_face(ex->bottom, face_shader, V3_to_V4(extruder_COLOR, 0.1), rot, view, proj, false);

	glClear(GL_DEPTH_BUFFER_BIT);
	render_face(ex->top, face_shader, ex->hovered ? V3_to_V4(extruder_COLOR, 0.5) : V3_to_V4(extruder_COLOR, 0.1), rot, view, proj, false);

	for (i = 0; i < ex->side_count; i++) {
		glClear(GL_DEPTH_BUFFER_BIT);
		render_side_face(ex, ex->sides[i], face_shader, V3_to_V4(extruder_COLOR, 0.1), rot, view, proj, ex->invert_wind);
		//if (show_border) {
		//	glClear(GL_DEPTH_BUFFER_BIT);
		//	render_side_face_border(ex, ex->sides[i], border_shader, V3_to_V4(extruder_COLOR, 1.0), view, proj, ex->invert_wind);
		//}
	}

	if (show_border) {
		glClear(GL_DEPTH_BUFFER_BIT);
		render_face_border(ex->bottom, border_shader, V3_to_V4(extruder_COLOR, 1.0), view, proj, false);
		glClear(GL_DEPTH_BUFFER_BIT);
		render_face_border(ex->top, border_shader, V3_to_V4(extruder_COLOR, 1.0), view, proj, false);
	}
}
