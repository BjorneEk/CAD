/*==========================================================*
 *
 * @author Gustaf Franzén :: https://github.com/BjorneEk;
 *
 * plane representation
 *
 *==========================================================*/

#ifndef _PLANE_H_
#define _PLANE_H_

#include "matrix/vector.h"
#include "GL/shader.h"
#include "GL/font.h"

typedef struct plane {
	v3f_t normal;
	v3f_t axis_1;
	v3f_t pos;
	f32_t scale;
	bool hovered;
	bool selected;
	bool visible;
} plane_t;

typedef struct axis {
	v3f_t dir;
	v3f_t pos;
	f32_t scale;
	bool hovered;
	bool selected;
	bool visible;
} axis_t;

plane_t *new_plane(v3f_t normal, v3f_t axis_1, v3f_t pos, f32_t scale);

void show_plane(plane_t *plane, shader_t shader, v3f_t color, f32_t *view, f32_t *proj);

axis_t *new_axis(v3f_t dir, v3f_t pos, f32_t scale);

void show_axis(axis_t *axis, shader_t shader, v3f_t color, f32_t *view, f32_t *proj);

bool is_plane(plane_t *plane, v3f_t real_pos);

//bool is_plane_(plane_t *plane, v3f_t real_pos);

// v3f_t orthagonal_project(plane_t *p, v3f_t u);

v3f_t project(v3f_t normal, v3f_t pos, v3f_t dir);

f32_t *look_at_plane(plane_t *plane);


void _show_plane(plane_t *plane, shader_t shader, v3f_t color, f32_t *view, f32_t *proj, bool selected, bool hovered);

#endif /* _PLANE_H_ */
