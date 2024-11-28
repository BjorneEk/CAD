/*==========================================================*
 *
 * @author Gustaf Franzén :: https://github.com/BjorneEk;
 *
 *
 *
 *==========================================================*/

#include "draw.h"
#include "../util/geometry.h"
#include "../util/glutils.h"
#include "../GL/shader.h"
#include "../matrix/matrix.h"

#include <stdlib.h>
#include <math.h>
#include <glad/glad.h>
#include <stdio.h>



static const char *DOT_VERT_PATH = "src/shaders/dot_vert.glsl";
static const char *DOT_FRAG_PATH = "src/shaders/dot_frag.glsl";

static u32_t DOT_FLOAT_COUNT;

static shader_t dot_shader()
{
	static shader_t shader;
	static bool shader_loaded = false;

	if (shader_loaded)
		return shader;
	else
		shader_load(&shader, DOT_VERT_PATH, DOT_FRAG_PATH);
	shader_loaded = true;
	return shader;
}

static GL_object_t dot_obj()
{
	f32_t *dot_mesh;
	static bool generated = false;
	static GL_object_t res;

	if (generated)
		return res;

	dot_mesh = gen_sector(V2(0.0, 0.0), 0.5, 0, 2 * M_PI, 20);
	DOT_FLOAT_COUNT = 20 * 3 * 2;
	res = V2_VAO(dot_mesh, DOT_FLOAT_COUNT);
	free(dot_mesh);
	generated = true;
	return res;
}
/*
void draw_dot(v3f_t p, f32_t radius, v4f_t color, f32_t *view, f32_t *proj)
{
	shader_t shader;
	u32_t VAO;
	shader = dot_shader();
	VAO = dot_VAO();


	shader_use(&shader);
	shader_setV3(&shader, "pos", p);
	shader_setV4(&shader, "color", color);
	shader_setM4(&shader, "projection", proj);
	shader_setM4(&shader, "view", view);
	shader_setf(&shader, "radius", radius);

	glClear(GL_DEPTH_BUFFER_BIT);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	glDisable(GL_CULL_FACE);


	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, DOT_FLOAT_COUNT);
}*/
void draw_dot_normal(v3f_t p, v3f_t normal, f32_t radius, v4f_t color, f32_t *view, f32_t *proj)
{
	shader_t shader;
	GL_object_t obj;
	f32_t *dir_mat;

	shader = dot_shader();
	obj = dot_obj();
	//dir_mat = M4_rotation(V3(0.0, 1.0, 0.0), M_PI / 2);
	dir_mat = rotational_transform(normal, V3(0.0, 0.0, 1.0));
	//M4_mult(view, dir_mat);

	shader_use(&shader);
	shader_setV3(&shader, "pos", p);
	shader_setV4(&shader, "color", color);
	shader_setM4(&shader, "projection", proj);
	shader_setM4(&shader, "view", view);
	shader_setM4(&shader, "rot", dir_mat);
	shader_setf(&shader, "radius", radius);

	glClear(GL_DEPTH_BUFFER_BIT);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	glDisable(GL_CULL_FACE);


	glBindVertexArray(obj.VAO);
	glDrawArrays(GL_TRIANGLES, 0, DOT_FLOAT_COUNT);
	free(dir_mat);
}