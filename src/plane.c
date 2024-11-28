/*==========================================================*
 *
 * @author Gustaf Franzén :: https://github.com/BjorneEk;
 *
 * plane representation
 *
 *==========================================================*/

#include "plane.h"
#include "util/glutils.h"
#include "GL/globject.h"
#include "matrix/vector.h"
#include "matrix/matrix.h"
#include "GL/shader.h"
#include <stdlib.h>
#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <math.h>


plane_t *new_plane(v3f_t normal, v3f_t axis_1, v3f_t pos, f32_t scale)
{
	plane_t *res;

	res = malloc(sizeof(plane_t));
	res->normal = V3_normalized(normal);
	res->axis_1 = V3_normalized(axis_1);
	res->pos = pos;
	res->scale = scale;
	res->selected = false;
	res->hovered = false;
	res->visible = true;
	return res;
}
//sketch_t *begin_sketch(plane_t *plane)
//{
//	return new_sketch(selectable(PLANE, plane), );
//}

axis_t *new_axis(v3f_t dir, v3f_t pos, f32_t scale)
{
	axis_t *res;

	res = malloc(sizeof(axis_t));
	res->dir = V3_normalized(dir);
	res->pos = pos;
	res->scale = scale;
	res->visible = true;
	return res;
}

void _show_plane(plane_t *plane, shader_t shader, v3f_t color, f32_t *view, f32_t *proj, bool selected, bool hovered)
{
	v3f_t axis_2;
	GL_object_t mesh_obj;
	GL_object_t edge_obj;

	V3_normalize(&plane->axis_1);
	axis_2 = V3_cross(plane->normal, plane->axis_1);
	V3_normalize(&axis_2);
	axis_2 = V3_scale(axis_2, plane->scale / 2.0);
	plane->axis_1 = V3_scale(plane->axis_1, 0.5);

	v3f_t mesh[6] = {
		V3_sub(V3_add(plane->pos, plane->axis_1), axis_2), // tl
		V3_sub(V3_sub(plane->pos, plane->axis_1), axis_2), // bl
		V3_add(V3_sub(plane->pos, plane->axis_1), axis_2), // br
		V3_sub(V3_add(plane->pos, plane->axis_1), axis_2),
		V3_add(V3_sub(plane->pos, plane->axis_1), axis_2),
		V3_add(V3_add(plane->pos, plane->axis_1), axis_2) // tr
	};

	v3f_t edges[8] = {
		mesh[0], mesh[5],
		mesh[0], mesh[1],
		mesh[1], mesh[2],
		mesh[2], mesh[5]
	};

	mesh_obj = V3_VAO((f32_t*)mesh, 18);
	edge_obj = V3_VAO((f32_t*)edges, 24);


	shader_use(&shader);
	shader_setV4(&shader, "color", V4(color.x, color.y, color.z, selected ? 0.3 : (hovered ? 0.2 : 0.1)));
	shader_setM4(&shader, "projection", proj);
	shader_setM4(&shader, "view", view);

	glClear(GL_DEPTH_BUFFER_BIT);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	glDisable(GL_CULL_FACE);

	glBindVertexArray(mesh_obj.VAO);
	glDrawArrays(GL_TRIANGLES, 0, mesh_obj.length);
	//glDeleteVertexArrays(1, &mesh_obj.VAO);
	//glDeleteBuffers(1, &mesh_obj.VBO);

	shader_setV4(&shader, "color", V4(color.x, color.y, color.z, selected ? 0.9 : (hovered ? 0.7 : 0.5)));

	glBindVertexArray(edge_obj.VAO);
	glDrawArrays(GL_LINES, 0, edge_obj.length);
	//glDeleteVertexArrays(1, &edge_obj.VAO);
	//glDeleteBuffers(1, &edge_obj.VBO);
	glBindVertexArray(0);

	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);
}

void show_plane(plane_t *plane, shader_t shader, v3f_t color, f32_t *view, f32_t *proj)
{
	_show_plane(plane, shader, color, view, proj, plane->selected, plane->hovered);
}

// v3f_t orthagonal_project(plane_t *p, v3f_t u)
// {
// 	v3f_t u_;

// 	u_ = V3_scale(p->normal, V3_dot(p->normal, u));

// 	return V3_sub(u, u_);
// }

v3f_t project(v3f_t normal, v3f_t pos, v3f_t dir)
{
	v3f_t orth;
	v3f_t res;

	V3_normalize(&dir);

	res = V3_scale(dir, V3_dot(dir, V3_scale(normal, V3_dot(normal, pos))));

	return V3_sub(pos, orth);
}

// bool triangle_contains(v3f_t *trig, v3f_t p) {

// 	f64_t denominator;
// 	f64_t a;
// 	f64_t b;
// 	f64_t c;
// 	denominator = ((trig[1].y - trig[2].y) * (trig[0].x - trig[2].x) + (trig[2].x - trig[1].x) * (trig[0].y - trig[2].y));
// 	a = ((trig[1].y - trig[2].y) * (p.x - trig[2].x) + (trig[2].x - trig[1].x) * (p.y - trig[2].y)) / denominator;
// 	b = ((trig[2].y - trig[0].y) * (p.x - trig[2].x) + (trig[0].x - trig[2].x) * (p.y - trig[2].y)) / denominator;
// 	c = 1 - a - b;

// 	return (a >= 0 && b >= 0 && c >= 0);
// }


bool is_plane(plane_t *plane, v3f_t real_pos)
{
	v3f_t axis_2;

	V3_normalize(&plane->axis_1);
	axis_2 = V3_cross(plane->normal, plane->axis_1);
	V3_normalize(&axis_2);
	axis_2 = V3_scale(axis_2, plane->scale * 0.5);
	plane->axis_1 = V3_scale(plane->axis_1, 0.5);

	v3f_t tri_1[3] = {
		V3_add(V3_add(plane->pos, plane->axis_1), axis_2),
		V3_add(V3_sub(plane->pos, plane->axis_1), axis_2),
		V3_sub(V3_sub(plane->pos, plane->axis_1), axis_2)
	};
	v3f_t tri_2[3] = {
		V3_add(V3_add(plane->pos, plane->axis_1), axis_2),
		V3_sub(V3_sub(plane->pos, plane->axis_1), axis_2),
		V3_sub(V3_add(plane->pos, plane->axis_1), axis_2)
	};

	return triangle_contains(tri_1, real_pos)
		|| triangle_contains(tri_2, real_pos);

}


// bool is_plane(plane_t *plane, f32_t *view, f32_t *proj, v2f_t pos)
// {
// 	//used_pos = (V3_len(V3_sub(projected_pos, real_pos)) < 0.01) ? projected_pos : real_pos;
// 	v3f_t axis_2;
// 	v3f_t axis_2_proj;
// 	v3f_t axis_1_proj;
// 	v3f_t pos_proj;
// 	f32_t *vp;

// 	V3_normalize(&plane->axis_1);
// 	axis_2 = V3_cross(plane->normal, plane->axis_1);
// 	V3_normalize(&axis_2);
// 	axis_2 = V3_scale(axis_2, plane->scale * 0.5);
// 	plane->axis_1 = V3_scale(plane->axis_1, 0.5);

// 	vp = M4_eye();
// 	M4_mult(vp, proj);
// 	M4_mult(vp, view);
// 	axis_1_proj = M4_mult_V3(vp, plane->axis_1);
// 	axis_2_proj = M4_mult_V3(vp, axis_2);
// 	pos_proj = M4_mult_V3(vp, plane->pos);
// 	free(vp);
// 	//printf("DET: %f\n", M4_det(vp));

// 	v3f_t tri_1[3] = {
// 		V3_add(V3_add(pos_proj, axis_1_proj), axis_2_proj),
// 		V3_add(V3_sub(pos_proj, axis_1_proj), axis_2_proj),
// 		V3_sub(V3_sub(pos_proj, axis_1_proj), axis_2_proj)
// 	};

// 	v3f_t tri_2[3] = {
// 		V3_add(V3_add(pos_proj, axis_1_proj), axis_2_proj),
// 		V3_sub(V3_sub(pos_proj, axis_1_proj), axis_2_proj),
// 		V3_sub(V3_add(pos_proj, axis_1_proj), axis_2_proj)
// 	};

// 	//printf("POS: %f, %f\n", pos.x, pos.y);
// 	//printf("x1: %f, y1: %f, x2: %f, y2: %f\n", bounding_box[0].x, bounding_box[0].y, bounding_box[1].x, bounding_box[1].y);
// 	//printf("x3: %f, y3: %f, x4: %f, y4: %f\n---\n", bounding_box[2].x, bounding_box[2].y, bounding_box[3].x, bounding_box[3].y);
// 	//for(int i = 0; i < 4; i++) {
// 	//	printf("x: %f, y: %f, z: %f\n", bounding_box[i].x, bounding_box[i].y, bounding_box[i].z);
// 	//}
// 	return triangle_contains(tri_1, V2_to_V3(pos, 0.0))
// 		|| triangle_contains(tri_2, V2_to_V3(pos, 0.0));

// }



f32_t *look_at_plane(plane_t *plane)
{
	/* already looking at plane, would cause float problems */
	//if (plane->normal.x == 0.0 && plane->normal.y == 0.0)
	//	return M4_eye();

	//return M4_rotation(V3(plane->normal.y, -plane->normal.x, 0.0), acos(plane->normal.z));
	return rotational_transform(plane->normal,V3(0.0, 0.0, 1.0));

}



void show_axis(axis_t *axis, shader_t shader, v3f_t color, f32_t *view, f32_t *proj)
{
	GL_object_t obj;

	V3_normalize(&axis->dir);
	axis->dir = V3_scale(axis->dir, axis->scale * 0.5);
	v3f_t edges[2] = {
		V3_sub(axis->pos, axis->dir),
		V3_add(axis->pos, axis->dir)
	};

	obj = V3_VAO((f32_t*)edges, 6);

	shader_use(&shader);
	shader_setV4(&shader, "color", V4(color.x, color.y, color.z, 0.8));
	shader_setM4(&shader, "projection", proj);
	shader_setM4(&shader, "view", view);

	glClear(GL_DEPTH_BUFFER_BIT);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	glDisable(GL_CULL_FACE);

	glBindVertexArray(obj.VAO);
	glDrawArrays(GL_LINES, 0, obj.length);
	glBindVertexArray(0);
	//glDeleteVertexArrays(1, &obj.VAO);
	//glDeleteBuffers(1, &obj.VBO);


	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);
}
