/*==========================================================*
 *
 * @author Gustaf Franzén :: https://github.com/BjorneEk;
 *
 * cad program state struct
 *
 *==========================================================*/

#ifndef _STATE_H_
#define _STATE_H_

#include "sketch/sketch.h"
#include "GL/shader.h"
#include "object/extruder.h"
#include "GL/font.h"
#include "structures/dynamic_array.h"
#include "plane.h"
#include "GUI/GUI.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>


typedef enum projection_type {
	PERSPECTIVE_PROJECTION	= 1,
	ORTHOGRAPHIC_PROJECTION	= 0,
} proj_type_t;

typedef enum key_state {
	KEY_DOWN = 1,
	KEY_UP = 0
} key_state_t;


typedef struct menu {
	GUI_ctx *main;
	GUI_ctx *sketch;
	GUI_ctx *extrude;
	v2f_t start_pos;

	GUI_palette *regular;
	GUI_palette *focused;
	GUI_palette *hovered;
	GUI_palette *pressed;
	GUI_palette *toggled;

	GUI_palette *regular_txt;
	GUI_palette *focused_txt;
	GUI_palette *hovered_txt;
	GUI_palette *pressed_txt;
	GUI_palette *toggled_txt;

	GUI_palette *regular_red;
	GUI_palette *focused_red;
	GUI_palette *hovered_red;
	GUI_palette *pressed_red;
	GUI_palette *toggled_red;

	GUI_component *start_sketch_btn;
	GUI_component *extrude_btn;
	GUI_component *projection_tgl;
	GUI_component *show_edges_tgl;

	GUI_component *sketch_exit_btn;
	GUI_component *line_btn;
	GUI_component *delete_selected_btn;
	GUI_component *finish_sketch_btn;
	GUI_component *dimension_btn;

	GUI_component *extrude_exit_btn;
	GUI_component *extrude_select_btn;
	GUI_component *extrude_dist_lbl;
	GUI_component *extrude_dist_text_field;
	GUI_component *extrude_finish_btn;

	u32_t ctx_count;
	GUI_ctx *contexts[3];
} menu_t;
enum {
	MODE_NORMAL,
	MODE_IN_SKETCH,
	MODE_ENTERING_SKETCH,
	MODE_DRAWING_LINE,
	MODE_IN_EXTRUDE
};

typedef struct program_state {
	GLFWwindow*	window;

	u32_t	screen_width;
	u32_t	screen_height;
	f32_t	fov;

	GLFWcursor	*cursor_crosshair;
	GLFWcursor	*cursor_hand;
	GLFWcursor	*cursor_arrow;
	GLFWcursor	*cursor_ibeam;
	GLFWcursor	*cursor_current;

	font_t	*font;

	f32_t	*font_proj;

	f32_t	*rotation;
	f32_t	*translation;
	f32_t	*scale;

	proj_type_t	projection_type;

	struct {
		u8_t	anti_aliasing		: 1;
		u8_t	in_scetch		: 1;
		u8_t	focus_animating		: 1;
		u8_t	selecting_geometry	: 1;
	}	flags;

	bool show_edges;

	shader_t	plane_shader;
	shader_t	obj_shader;
	shader_t	extruder_shader;

	dla_t		*content;	/* slectable_t	*/
	dla_t		*planes;	/* plane_t*	*/
	dla_t		*faces;		/* face_t*	*/
	dla_t		*axes;		/* axle_t*	*/
	dla_t		*segments;	/* segment_t*	*/
	dla_t		*objects;	/*object_t*	*/

	selectable_t *selected;
	selectable_t *hovered;



	sketch_t *sketch;
	extruder_t *extruder;


	f32_t snap_dist;

	struct {
		key_state_t lshift;
		key_state_t rshift;
	} keys;

	struct {
		f32_t x;
		f32_t y;
		f32_t prev_x;
		f32_t prev_y;
		i32_t state_left;
		i32_t state_right;
		bool first;
	} mouse;

	i32_t mode;

	menu_t *menu;

	v3f_t hover_point;
	v3f_t hover_point_normal;
	bool show_hover_point;
	bool has_hover_point;

} program_state_t;

program_state_t *new_program(GLFWwindow *window, u32_t width, u32_t height, f32_t fov_perspective);

v3f_t get_normal(selectable_t s);
v3f_t get_pos(selectable_t s);

void update_mouse_pos(program_state_t *p, f64_t x_in, f64_t y_in);

void update_hovered(program_state_t *p, v3f_t inv_mouse);
void hover(program_state_t *p, selectable_t *s, v3f_t pos);
void unhover(program_state_t *p);
void select(program_state_t *p);
void deselect(program_state_t *p);

void set_extruder_extrude_dist(program_state_t*p, f32_t dist);
void enter_sketch(program_state_t *p, selectable_t parent);
void exit_sketch(program_state_t *p);

bool is_hovering_plane(program_state_t *p, selectable_t *s, v3f_t pos, v3f_t dir);
bool is_hovering_cycle(program_state_t *p, selectable_t *s, v3f_t pos, v3f_t dir);
bool is_hovering_face(program_state_t *p, selectable_t *s, v3f_t pos, v3f_t dir);
bool is_hovering(program_state_t *p, selectable_t *s, v3f_t pos, v3f_t dir);

bool is_sketching(program_state_t *p);

void	draw_selectable(program_state_t *p, selectable_t s, f32_t *vi, f32_t *pr);
void	add_selectable(program_state_t *p, selectable_t s);

void	render_content(program_state_t *p, f32_t *vi, f32_t *pr);

f32_t	*view(program_state_t *p);
f32_t	*view_inv(program_state_t *p);
f32_t	*projection(program_state_t *prog);
f32_t	*projection_inv(program_state_t *prog);

f32_t	*viewproj(program_state_t *prog);
f32_t	*viewproj_inv(program_state_t *prog);

v3f_t	invert_direction(program_state_t *p, v3f_t dir);
v3f_t	invert_notranslation(program_state_t *p, v3f_t pos);

void	reset_view_soft(program_state_t *p);
void	reset_view_hard(program_state_t *p);

void	init_menu(program_state_t *p);
bool	menu_contains(program_state_t *prog, v2f_t p);
void	menu_mouse_button_update(program_state_t *p, i32_t button, i32_t action, i32_t mods);
void	menu_mouse_motion_update(program_state_t *p, f64_t x, f32_t y);
void	menu_text_input_update(program_state_t *p, u32_t codepoint);
void	menu_key_update(program_state_t *p, i32_t key, i32_t scancode, i32_t action, i32_t mods);
void	menu_draw(program_state_t *prog);

void try_snap(program_state_t *prog, v3f_t *pos);

v3f_t project_point(program_state_t *prog, v3f_t p);

#endif /* _STATE_H_ */