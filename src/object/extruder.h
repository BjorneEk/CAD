/*==========================================================*
 *
 * @author Gustaf Franzén :: https://github.com/BjorneEk;
 *
 * extrusions
 *==========================================================*/

#ifndef _EXTRUDE_H_
#define _EXTRUDE_H_

#include "object.h"

typedef struct extruder_side_face {
	v3f_t top[2];
	v3f_t bottom[2];
} side_face_t;

typedef struct extruder {

	selectable_t *parent;

	face_t *bottom;
	face_t *top;

	v3f_t normal;
	f32_t dist;
	bool has_parent;

	side_face_t *sides;
	u32_t side_count;

	bool invert_wind;

	v3f_t hovered_pos;
	bool pressed;
	bool hovered;
} extruder_t;

extruder_t *new_extruder();
void	free_extruder(extruder_t **ex, bool has_extruded);
void	extruder_set_dist(extruder_t *ex, f32_t dist);
v3f_t	extruder_project(extruder_t *e, v3f_t inv_mouse, v3f_t look_dir);
void	set_extrude_geometry(extruder_t *e, selectable_t *s);
void	render_extruder(extruder_t *ex, shader_t face_shader, shader_t border_shader, f32_t *rot, f32_t *view, f32_t *proj, bool show_border);
object_t *finish_extrusion(extruder_t *ex);
#endif /* _EXTRUDE_H_ */