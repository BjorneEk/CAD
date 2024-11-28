/*==========================================================*
 *
 * @author Gustaf Franzén :: https://github.com/BjorneEk;
 *
 * object
 *
 *==========================================================*/

#ifndef _OBJECT_H_
#define _OBJECT_H_

#include "../sketch/sketch.h"
#include "../plane.h"
#include <glad/glad.h>

typedef struct object object_t;

typedef struct face {
	v3f_t normal;
	v3f_t pos;

	v3f_t *verts;
	u64_t vert_count;

	u32_t *mesh;
	u64_t mesh_len;

	line_t *border;
	u64_t border_len;

	u32_t VAO;
	u32_t VBO;
	u32_t EBO;
} face_t;

typedef struct object {
	v3f_t *mesh;
	u64_t mesh_length;

	face_t *faces;
	u64_t face_count;

	selectable_t *parent;
} object_t;

void free_face(face_t **f);
object_t *new_object(face_t *faces, u64_t face_count, selectable_t *parent);
void draw_border_line(line_t l, shader_t shader, v4f_t color, f32_t *view, f32_t *proj);
bool face_contains(face_t *f, v3f_t p);
face_t *new_face(v3f_t pos, v3f_t *verts, u64_t vert_cnt, u32_t *mesh, u64_t mesh_len, line_t *border, u64_t border_len);
void render_face(face_t *f, shader_t shader, v4f_t color, f32_t *rot, f32_t *view, f32_t *proj, bool cull_face);
void render_face_border(face_t *f, shader_t shader, v4f_t color, f32_t *view, f32_t *proj, bool show_normal);
void render_object(object_t *o, shader_t shader,shader_t border_shader, v4f_t color, v4f_t border_color, f32_t *rot, f32_t *view, f32_t *proj, bool show_edges);
//object_t *extrude(cycle_t *c, f32_t dist);
void render_objects(dla_t *objs, shader_t shader,shader_t border_shader, v4f_t color, v4f_t border_color, f32_t *rot, f32_t *view, f32_t *proj, bool show_edges);
//void show_object(object_t *obj, shader_t shader, f32_t *rot, f32_t *view, f32_t *proj);

#endif /* _OBJECT_H_ */