/*==========================================================*
 *
 * @author Gustaf Franzén :: https://github.com/BjorneEk;
 *
 * cad program state struct
 *
 *==========================================================*/

#include "object/extruder.h"
#include "selectable/selectable.h"
#include "sketch/segment.h"
#include "util/types.h"
#include "util/error.h"
#include "GL/shader.h"
#include "GL/draw.h"
#include "GL/font.h"
#include "matrix/matrix.h"
#include "matrix/vector.h"
#include "structures/dynamic_array.h"
#include "GUI/GUI.h"
#include "GUI/GUI_component.h"
#include "program_state.h"
#include "plane.h"
#include "sketch/sketch.h"
#include "sketch/sketch_graph.h"
#include "object/object.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
/**
 * creates a pluss shaped crotthair cursor
 * @size the width ot the cursor, this needs to be an uneaven number
 **/

GLFWcursor* create_crosshair_cursor(u32_t size)
{
	u32_t *pixel_buffer;
	i32_t i;
	i32_t mid;

	pixel_buffer = calloc(size * size, sizeof(u32_t));

	mid = (size / 2);

	for (i = 0; i < size; i++) {
		pixel_buffer[mid + i * size] = 0xFF010101;
		pixel_buffer[mid * size + i] = 0xFF010101;
	}

	GLFWimage image;
	image.width = size;
	image.height = size;
	image.pixels = (u8_t*)pixel_buffer;

	return glfwCreateCursor(&image, mid, mid);
}

f64_t	rad(f64_t deg)
{
	const f64_t rads_per_deg = (M_PI / 180.0);
	return deg * rads_per_deg;
}

static void add_orgin_planes(program_state_t *p, shader_t shader)
{
	selectable_t tmp;

	tmp = selectable(PLANE, new_plane(V3(0.0, 0.0, 1.0), V3(1.0, 0.0, 0.0), V3(0.0, 0.0, 0.0), 1), NULL);
	add_selectable(p, tmp);
	tmp = selectable(PLANE, new_plane(V3(0.0, 1.0, 0.0), V3(1.0, 0.0, 0.0), V3(0.0, 0.0, 0.0), 1), NULL);
	add_selectable(p, tmp);
	tmp = selectable(PLANE, new_plane(V3(1.0, 0.0, 0.0), V3(0.0, 1.0, 0.0), V3(0.0, 0.0, 0.0), 1), NULL);
	add_selectable(p, tmp);
}

static void add_orgin_axes(program_state_t *p, shader_t shader)
{
	selectable_t tmp;

	tmp = selectable(AXIS, new_axis(V3(1.0, 0.0, 0.0), V3(0.0, 0.0, 0.0), 1.8), NULL);
	add_selectable(p, tmp);
	tmp = selectable(AXIS, new_axis(V3(0.0, 1.0, 0.0), V3(0.0, 0.0, 0.0), 1.8), NULL);
	add_selectable(p, tmp);
	tmp = selectable(AXIS, new_axis(V3(0.0, 0.0, 1.0), V3(0.0, 0.0, 0.0), 1.8), NULL);
	add_selectable(p, tmp);
}

f32_t *perspective(program_state_t *prog)
{
	return M4_perspective(rad(prog->fov), (f32_t)prog->screen_width/(f32_t)prog->screen_height, 0.1, 100.0);
}

f32_t *perspective_inv(program_state_t *prog)
{
	return M4_inverse_perspective(rad(prog->fov), (f32_t)prog->screen_width/(f32_t)prog->screen_height, 0.1, 100.0);
}

f32_t *orthograpic(program_state_t *prog)
{
	return M4_orthographic(-1.0*prog->screen_height/prog->screen_width, 1.0*prog->screen_height/prog->screen_width, -1.0, 1.0);
}

f32_t *orthograpic_inv(program_state_t *prog)
{
	return M4_inverse_orthographic(-1.0*prog->screen_height/prog->screen_width, 1.0*prog->screen_height/prog->screen_width, -1.0, 1.0);
}

f32_t *projection(program_state_t *prog)
{
	return prog->projection_type == ORTHOGRAPHIC_PROJECTION ? orthograpic(prog) : perspective(prog);
}

f32_t *projection_inv(program_state_t *prog)
{
	return prog->projection_type == ORTHOGRAPHIC_PROJECTION ? orthograpic_inv(prog) : perspective_inv(prog);
}


void	reset_view_soft(program_state_t *p)
{
	free(p->translation);
	free(p->scale);
	p->scale = M4_eye();
	p->translation = M4_eye();
	if (p->projection_type == PERSPECTIVE_PROJECTION) {
		p->translation[13] = -0.4;
		p->translation[14] = -3.0;
	}
}
void	reset_view_hard(program_state_t *p)
{
	reset_view_soft(p);
	free(p->rotation);
	p->rotation = M4_eye();
	p->scale = M4_eye();

}
f32_t	*view(program_state_t *p)
{
	f32_t *res;

	res = M4_eye();

	if (p->projection_type == ORTHOGRAPHIC_PROJECTION)
		M4_mult(res, p->scale);

	M4_mult(res, p->translation);

	M4_mult(res, p->rotation);

	return res;
}

f32_t	*view_inv(program_state_t *p)
{
	f32_t *res;

	res = M4_eye();

	M4_invert_rotation(res, p->rotation);

	M4_invert_translation(res, p->translation);

	if (p->projection_type == ORTHOGRAPHIC_PROJECTION)
		M4_invert_scaling(res, p->scale);


	return res;
}

f32_t	*viewproj(program_state_t *prog)
{
	f32_t *res;
	f32_t *v;

	v = view(prog);
	res = projection(prog);
	M4_mult(res, v);
	free(v);
	return res;

}

f32_t	*viewproj_inv(program_state_t *prog)
{
	f32_t *res;
	f32_t *proj;

	res = view_inv(prog);
	proj = projection_inv(prog);
	M4_mult(res, proj);
	free(proj);
	return res;
}
v3f_t project_point(program_state_t *prog, v3f_t p)
{
	f32_t *proj, *rot, *trans, *scale;
	v3f_t res;


	proj	= projection(prog);
	res = M4_mult_V3(prog->rotation, p);
	res = M4_mult_V3(prog->translation, res);


	if (prog->projection_type == ORTHOGRAPHIC_PROJECTION) {
		res = M4_mult_V3(prog->scale, res);
	}
	res = M4_mult_V3(proj, res);

	free(proj);

	return res;
}

void try_snap(program_state_t *prog, v3f_t *pos)
{
	v3f_t proj;
	v3f_t dir_scaled;
	try_snap_sketch(prog->sketch, pos, prog->snap_dist);
	FOREACH_DLA(prog->axes, i, axis_t*, ax, {
		dir_scaled = V3_scale(ax->dir, 0.5 * ax->scale);
		proj = V3_proj_point_line(V3_sub(ax->pos, dir_scaled), V3_add(ax->pos, dir_scaled), *pos);
		if (V3_len(V3_sub(proj, *pos)) <= prog->snap_dist) {
			*pos = proj;
		}
	});
}



program_state_t *new_program(GLFWwindow *window, u32_t width, u32_t height, f32_t fov_perspective)
{
	program_state_t *res;

	res = malloc(sizeof(program_state_t));
	res->window		= window;
	res->screen_width	= width;
	res->screen_height	= height;
	res->fov		= 45.0f;

	res->cursor_crosshair	= create_crosshair_cursor(17);
	res->cursor_hand	= glfwCreateStandardCursor(GLFW_HAND_CURSOR);
	res->cursor_arrow	= glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
	res->cursor_ibeam	= glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
	res->cursor_current	= res->cursor_crosshair;

	res->font	= new_font("/System/Library/Fonts/Monaco.ttf");
	res->font_proj	= M4_orthographic(0.0, height, 0.0, width);

	res->projection_type	= ORTHOGRAPHIC_PROJECTION;
	res->rotation	= M4_eye();
	res->translation	= M4_eye();
	res->scale		= M4_eye();

	res->flags.anti_aliasing = 1;
	res->flags.in_scetch = 0;
	res->flags.focus_animating = 0;

	shader_load(&res->plane_shader, "src/shaders/plane_vert.glsl", "src/shaders/plane_frag.glsl");
	shader_load(&res->obj_shader, "src/shaders/obj_vert.glsl", "src/shaders/obj_frag.glsl");
	shader_load(&res->extruder_shader, "src/shaders/obj_vert.glsl", "src/shaders/extruder_frag.glsl");

	res->content		= DLA_new(sizeof(selectable_t), 10);
	res->planes		= DLA_new(sizeof(plane_t*), 3);
	res->segments		= DLA_new(sizeof(segment_t*), 5);
	res->axes		= DLA_new(sizeof(axis_t*), 3);
	res->objects		= DLA_new(sizeof(object_t*), 2);
	res->faces		= DLA_new(sizeof(face_t*), 10);
	res->show_edges = true;

	res->sketch = NULL;
	res->flags.selecting_geometry = 0;
	res->snap_dist = 0.01;
	memset(&res->keys, 0, sizeof(res->keys));

	res->mouse.x		= 0.0;
	res->mouse.y		= 0.0;
	res->mouse.prev_x	= 0.0;
	res->mouse.prev_y	= 0.0;
	res->mouse.state_left	= 0.0;
	res->mouse.state_right	= 0.0;
	res->mouse.first	= false;

	res->hovered = NULL;
	res->selected = NULL;

	res->hover_point = V3(0.0, 0.0, 0.0);
	res->has_hover_point = false;
	res->show_hover_point = false;
	res->extruder = NULL;

	res->mode = MODE_NORMAL;

	add_orgin_planes(res, res->plane_shader);
	add_orgin_axes(res, res->plane_shader);
	init_menu(res);
	return res;
}

v3f_t invert_direction(program_state_t *p, v3f_t dir)
{
	f32_t *inv_proj, *inv_rot;
	v3f_t res;

	inv_proj	= projection_inv(p);
	inv_rot		= M4_eye();

	M4_invert_rotation(inv_rot, p->rotation);

	res = M4_mult_V3(inv_proj, dir);
	res = M4_mult_V3(inv_rot, res);
	free(inv_proj);
	free(inv_rot);

	return res;
}

v3f_t invert_notranslation(program_state_t *p, v3f_t pos)
{
	f32_t *inv_proj, *inv_rot, *inv_scale;
	v3f_t res;

	inv_proj	= projection_inv(p);
	inv_rot		= M4_eye();

	M4_invert_rotation(inv_rot, p->rotation);

	res = M4_mult_V3(inv_proj, pos);
	if (p->projection_type == ORTHOGRAPHIC_PROJECTION) {
		inv_scale = M4_eye();
		M4_invert_scaling(inv_scale, p->scale);
		res = M4_mult_V3(inv_scale, res);
		free(inv_scale);
	}
	res = M4_mult_V3(inv_rot, res);

	free(inv_proj);
	free(inv_rot);

	return res;
}

bool is_sketching(program_state_t *p)
{
	return p->mode == MODE_IN_SKETCH || p->mode == MODE_DRAWING_LINE;
}

void update_mouse_pos(program_state_t *p, f64_t x_in, f64_t y_in)
{
	f32_t x;
	f32_t y;

	x = (x_in / p->screen_width) * 2 - 1;
	y = -((y_in / p->screen_height) * 2 - 1);

	if (p->mouse.first) {
		p->mouse.x = x;
		p->mouse.y = y;
		p->mouse.first = false;
	}

	p->mouse.prev_x = p->mouse.x;
	p->mouse.x = x;
	p->mouse.prev_y = p->mouse.y;
	p->mouse.y = y;
}

void update_extruder_dist_on_drag(program_state_t *p, v3f_t inv_mouse, v3f_t look_dir)
{
	v3f_t projected_mouse;
	if (p->mode == MODE_IN_EXTRUDE && p->extruder->has_parent) {
		projected_mouse = V3_raycast_plane(p->extruder->top->normal, p->extruder->top->pos, inv_mouse, look_dir);
		if (p->extruder->pressed) {
			set_extruder_extrude_dist(p, p->extruder->dist +
				V3_dot(p->extruder->normal, V3_sub(inv_mouse, p->extruder->hovered_pos)));
			p->extruder->hovered_pos = inv_mouse;
		}
		p->extruder->hovered = face_contains(p->extruder->top, projected_mouse);
		if (p->extruder->hovered || p->extruder->pressed) {
			p->extruder->hovered_pos = inv_mouse;
		}
	}
}


void update_hovered(program_state_t *p, v3f_t inv_mouse)
{
	v3f_t projected_mouse;
	v3f_t look_direction;


	look_direction = invert_direction(p, V3(0.0, 0.0, -1.0));
	unhover(p);
	FOREACH_DLA_PTR(p->content, i, selectable_t, s, {
		projected_mouse = V3_raycast_plane(
			get_normal(*s),
			get_pos(*s),
			inv_mouse,
			look_direction);


		if (is_hovering(p, s, projected_mouse, look_direction)) {
			hover(p, s, projected_mouse);
		}
		else {
			s->hovered = false;
		}
	});

	update_extruder_dist_on_drag(p, inv_mouse, look_direction);
}

void hover(program_state_t *p, selectable_t *s, v3f_t pos)
{
	p->hover_point = pos;
	p->has_hover_point = true;
	p->hover_point_normal = get_normal(*s);
	if (p->hovered != NULL) {
		p->hovered->hovered = false;
	}
	p->hovered = s;
	p->hovered->hovered = true;
}

void unhover(program_state_t *p)
{
	p->has_hover_point = false;
	if (p->hovered != NULL) {
		p->hovered->hovered = false;
	}
	if (p->extruder != NULL) {
		p->extruder->hovered = false;
	}
	p->hovered = NULL;
}

void select(program_state_t *p)
{
	if (p->selected != NULL) {
		p->selected->selected = false;
	}
	p->selected = p->hovered;
	p->selected->selected = true;
	if (p->mode == MODE_ENTERING_SKETCH) {
		enter_sketch(p, *p->selected);
		return;
	}
	if (p->flags.selecting_geometry && p->mode == MODE_IN_EXTRUDE && p->selected->type == CYCLE) {
		set_extrude_geometry(p->extruder, p->selected);
		p->flags.selecting_geometry = 0;
	}
}
void deselect(program_state_t *p)
{
	p->selected->selected = false;
	p->selected = NULL;
}

void set_extruder_extrude_dist(program_state_t*p, f32_t dist)
{
	char buff[10];
	sprintf(buff, "%.5f", dist);
	if (p->menu->extrude_dist_text_field->text.length < 10) {
		p->menu->extrude_dist_text_field->text.str = realloc(p->menu->extrude_dist_text_field->text.str, 10);
	}
	strcpy(p->menu->extrude_dist_text_field->text.str, buff);
	extruder_set_dist(p->extruder, dist);
}

void show_sketch_gui(program_state_t *p)
{
	p->menu->sketch->pos = p->menu->start_pos;
	p->menu->projection_tgl->enabled = false;
	p->menu->sketch->enabled = true;
}
void hide_sketch_gui(program_state_t *p)
{
	p->menu->sketch->enabled = false;
	p->menu->projection_tgl->enabled = true;
}

void enter_sketch(program_state_t *p, selectable_t parent)
{
	show_sketch_gui(p);
	p->mode = MODE_IN_SKETCH;
	if (p->projection_type == PERSPECTIVE_PROJECTION)
		GUI_toggle(p->menu->projection_tgl);

	p->menu->projection_tgl->enabled = false;
	p->flags.focus_animating = true;

	p->sketch = new_sketch(parent);
	p->flags.in_scetch = true;
}


void exit_sketch(program_state_t*p)
{
	dla_t *new_cycles; /* selectable_t */
	new_cycles = finish_sketch(&p->sketch);
	FOREACH_DLA(new_cycles, i, selectable_t, s, {
		add_selectable(p, s);
	});
	DLA_free(&new_cycles);

	hide_sketch_gui(p);
	p->mode = MODE_NORMAL;
	p->flags.in_scetch = false;
	p->sketch = NULL;
}

v3f_t get_normal(selectable_t s)
{
	if (s.type == FACE) {
		return s.face->normal;
	}
	if (s.type == PLANE) {
		return s.plane->normal;
	}
	if (s.type == CYCLE) {
		return get_normal(s.cycle->parent);
	}
	return V3(0.0, 0.0, 0.0);
}

v3f_t get_pos(selectable_t s)
{
	if (s.type == FACE) {
		return V3_add(s.face->verts[0], s.face->pos);
	}
	if (s.type == PLANE) {
		return s.plane->pos;
	}
	if (s.type == CYCLE) {
		return get_pos(s.cycle->parent);
	}
	return V3(0.0, 0.0, 0.0);
}

bool is_hovering_plane(program_state_t *p, selectable_t *s, v3f_t pos, v3f_t dir)
{
	return	(s->type == PLANE && is_plane(s->plane, pos) ) && (!p->has_hover_point || V3_dot(pos, dir) > V3_dot(p->hover_point, dir));
}
bool is_hovering_cycle(program_state_t *p, selectable_t *s, v3f_t pos, v3f_t dir)
{
	return s->type == CYCLE && (cycle_contains(s->cycle->cycle, pos) &&
		((!p->has_hover_point || V3_dot(pos, dir) > V3_dot(p->hover_point, dir)) || (p->hovered == NULL || p->hovered->type == PLANE)));
}

bool is_hovering_face(program_state_t *p, selectable_t *s, v3f_t pos, v3f_t dir)
{
	return s->type == FACE && (face_contains(s->face, pos) &&
		((!p->has_hover_point || V3_dot(pos, dir) > V3_dot(p->hover_point, dir)) || (p->hovered == NULL || p->hovered->type == PLANE || p->hovered->type == CYCLE)));
}
bool is_hovering(program_state_t *p, selectable_t *s, v3f_t pos, v3f_t dir)
{
	return is_hovering_plane(p, s, pos, dir) || is_hovering_cycle(p, s, pos, dir) || is_hovering_face(p, s, pos, dir);
}
void	add_selectable(program_state_t *p, selectable_t s)
{
	u32_t i;
	selectable_t s_;
	switch(s.type) {
		case UNCLOSED:
		case CYCLE:
			DLA_append(p->segments, &s.cycle);
			break;
		case PLANE:
			DLA_append(p->planes, &s.plane);
			break;
		case AXIS:
			DLA_append(p->axes, &s.axis);
			break;
		case OBJECT:
			DLA_append(p->objects, &s.object);
			for (i = 0; i < s.object->face_count; i++) {

				s_ = selectable(FACE, &s.object->faces[i], &s);
				DLA_append(p->content, &s_);
				DLA_append(p->faces, &s.object->faces[i]);
			}
			break;
	}
	DLA_append(p->content, &s);
}
void draw_selectable(program_state_t *p, selectable_t s, f32_t *vi, f32_t *pr)
{
	switch(s.type) {
		case UNCLOSED:
		case CYCLE:
			show_segment(s.cycle, p->plane_shader, p->font, vi, pr, s.selected, s.hovered);
			break;
		case PLANE:
			_show_plane(s.plane, p->plane_shader, V3(0.7, 0.7, 0.3), vi, pr, s.selected, s.hovered);
			break;
		case AXIS:
			show_axis(s.axis, p->plane_shader, V3(1.0, 1.0, 0.0), vi, pr);
			break;
		case OBJECT:
			//render_object(s.object, p->obj_shader,p->plane_shader, V4(0.9, 0.9, 0.9, 1.0), V4(0.0, 0.0, 0.0, 1.0), p->rotation, vi, pr, p->show_edges);
			break;
		case FACE:
			render_face(s.face, p->obj_shader, s.selected ? V4(0.90, 0.90, 1.0, 1.0) : s.hovered ? V4(0.85, 0.85, 0.85, 1.0) : V4(0.75, 0.75, 0.75, 1.0), p->rotation, vi, pr, true);
			if (p->show_edges)
				render_face_border(s.face, p->plane_shader, V4(0.0, 0.0, 0.0, 1.0), vi, pr, false);
			break;
	}
}

// void render_content(program_state_t *p, f32_t *vi, f32_t *pr)
// {
// 	dla_t *objs;

// 	objs = DLA_new(sizeof(object_t*), 1);

// 	FOREACH_DLA(p->content, i, selectable_t, s, {
// 		switch(s.type) {
// 			case UNCLOSED:
// 			case CYCLE:
// 				show_segment(s.cycle, p->plane_shader, p->font, vi, pr, s.selected, s.hovered);
// 				break;
// 			case PLANE:
// 				_show_plane(s.plane, p->plane_shader, V3(0.7, 0.7, 0.3), vi, pr, s.selected, s.hovered);
// 				break;
// 			case AXIS:
// 				show_axis(s.axis, p->plane_shader, V3(1.0, 1.0, 0.0), vi, pr);
// 				break;
// 			case OBJECT:
// 				DLA_append(objs, &s.object);
// 				break;
// 		}
// 	})
// 	if (objs->len > 0)
// 		render_objects(objs, p->obj_shader,p->plane_shader, V4(0.9, 0.9, 0.9, 1.0), V4(0.0, 0.0, 0.0, 1.0), p->rotation, vi, pr, p->show_edges);
// 	DLA_free(&objs);
// }