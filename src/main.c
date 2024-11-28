/*==========================================================*
 *
 * @author Gustaf Franzén :: https://github.com/BjorneEk;
 *
 * a simple CAD program
 *==========================================================*/




#include "object/extruder.h"
#include "selectable/selectable.h"
#include "selectable/selectable_types.h"
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
#include "listeners.h"
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



// global variables

f32_t	GLOBL_DELTA_TIME = 0;
static f32_t	GLOBL_FPS = 0;

static const u32_t	START_WIDTH	= 1920;
static const u32_t	START_HEIGHT	= 1080;

program_state_t *program;



static inline v2f_t	mouse_pos();
f32_t			delta_time();
v3f_t			inverse_project(v3f_t normal, v2f_t v);

void			text_input_callback(GLFWwindow *window, u32_t codepoint);
void			mouse_button_callback(GLFWwindow* window, i32_t button, i32_t action, i32_t mods);
void			mouse_motion_callback(GLFWwindow* window, f64_t x_in, f64_t y_in);
void			scroll_callback(GLFWwindow* window, f64_t x_in, f64_t y_in);
void			framebuffer_size_callback(GLFWwindow* window, i32_t width, i32_t height);
void			mouse_dragged(GLFWwindow* window, i32_t button, v2f_t pos);

void		update_fps(f32_t rate);
void		main_loop();
void		init_window(GLFWwindow** window, const char * title, u32_t width,
		u32_t height, void (*fsc)(GLFWwindow*, i32_t, i32_t));

v3f_t inverse(v3f_t p)
{
	f32_t *inv_proj, *inv_rot, *inv_trans, *inv_scale;
	v3f_t res;


	inv_proj	= projection_inv(program);
	inv_rot		= M4_eye();
	inv_trans	= M4_eye();

	M4_invert_translation(inv_trans, program->translation);
	M4_invert_rotation(inv_rot, program->rotation);

	res = M4_mult_V3(inv_proj, p);

	if (program->projection_type == ORTHOGRAPHIC_PROJECTION) {
		inv_scale = M4_eye();
		M4_invert_scaling(inv_scale, program->scale);
		res = M4_mult_V3(inv_scale, res);
		free(inv_scale);
	}

	res = M4_mult_V3(inv_trans, res);
	res = M4_mult_V3(inv_rot, res);


	free(inv_proj);
	free(inv_rot);
	free(inv_trans);

	return res;
}

v3f_t inverse_notrans(v3f_t p)
{
	f32_t *inv_proj, *inv_rot, *inv_scale;
	v3f_t res;

	inv_proj	= projection_inv(program);
	inv_rot		= M4_eye();

	M4_invert_rotation(inv_rot, program->rotation);

	res = M4_mult_V3(inv_proj, p);
	if (program->projection_type == ORTHOGRAPHIC_PROJECTION) {
		inv_scale = M4_eye();
		M4_invert_scaling(inv_scale, program->scale);
		res = M4_mult_V3(inv_scale, res);
		free(inv_scale);
	}
	res = M4_mult_V3(inv_rot, res);

	free(inv_proj);
	free(inv_rot);

	return res;
}

v3f_t inv_look_dir()
{
	f32_t *inv_proj, *inv_rot;
	v3f_t res;

	inv_proj	= projection_inv(program);
	inv_rot		= M4_eye();

	M4_invert_rotation(inv_rot, program->rotation);

	res = M4_mult_V3(inv_proj, V3(0.0, 0.0, 1.0));
	res = M4_mult_V3(inv_rot, res);
	free(inv_proj);
	free(inv_rot);

	return res;
}

v3f_t invert_rotation(v3f_t v)
{
	f32_t *inv_rot;
	v3f_t res;

	inv_rot = M4_eye();
	M4_invert_rotation(inv_rot, program->rotation);
	res = M4_mult_V3(inv_rot, res);
	free(inv_rot);
	return res;

}

v3f_t inverse_project(v3f_t normal, v2f_t v)
{
	return orthagonal_project(normal, inverse(V2_to_V3(v, 0.0)));
}

static inline v2f_t mouse_pos()
{
	return V2(program->mouse.x, program->mouse.y);
}

void toggle_anti_aliasing()
{
	if (!program->flags.anti_aliasing) {
		glEnable(GL_MULTISAMPLE);
		program->flags.anti_aliasing = 1;
	} else {
		glDisable(GL_MULTISAMPLE);
		program->flags.anti_aliasing = 0;
	}
}

void init_program(GLFWwindow *window, u32_t width, u32_t height, f32_t fov_perspective)
{
	i32_t i;

	program = new_program(window, width, height, fov_perspective);

	program->show_hover_point = false;

	for (i = 0; i < program->menu->ctx_count; i++) {
		FOREACH_DLA(program->menu->contexts[i]->components, j, GUI_component*, component, {
			if (component->type != GUI_LABEL) {
				GUI_add_listener(component, GUI_HOVERED, cursor_onhover_listener);
				GUI_add_listener(component, GUI_UNHOVERED, cursor_reset_listener);
			}
		});
	}

	GUI_add_listener(program->menu->projection_tgl,			GUI_TOGGLED,	toggle_projection_type_listener);
	GUI_add_listener(program->menu->show_edges_tgl,			GUI_TOGGLED,	toggle_show_edges_listener);
	GUI_add_listener(program->menu->start_sketch_btn,		GUI_CLICKED,	sketch_listener);
	GUI_add_listener(program->menu->extrude_btn,			GUI_CLICKED,	extrude_listener);

	GUI_add_listener(program->menu->delete_selected_btn,		GUI_CLICKED,	delete_selected_listener);
	GUI_add_listener(program->menu->sketch_exit_btn,		GUI_CLICKED,	exit_sketch_listener);
	GUI_add_listener(program->menu->finish_sketch_btn,		GUI_CLICKED,	exit_sketch_listener);
	GUI_add_listener(program->menu->line_btn,			GUI_CLICKED,	draw_line_listener);

	GUI_add_listener(program->menu->extrude_exit_btn,		GUI_CLICKED,	exit_extrude_listener);
	GUI_add_listener(program->menu->extrude_select_btn,		GUI_CLICKED,	select_extrude_geometry_listener);
	GUI_add_listener(program->menu->extrude_finish_btn,		GUI_CLICKED,	finish_extrusion_listener);
	GUI_add_listener(program->menu->extrude_dist_text_field,	GUI_TEXT_INPUT,	extrude_dist_updated_listener);
}

void text_input_callback(GLFWwindow *window, u32_t codepoint)
{
	menu_text_input_update(program, codepoint);
}

void keypress_callback(GLFWwindow* window, i32_t key, i32_t scancode, i32_t action, i32_t mods)
{
	f32_t *focus_view;
	plane_t *p;

	menu_key_update(program, key, scancode, action, mods);
	switch (key) {
		case GLFW_KEY_ESCAPE:
			if (action == GLFW_PRESS)
				switch(program->mode) {
					case MODE_ENTERING_SKETCH:
						program->mode = MODE_NORMAL;
						break;
					case MODE_DRAWING_LINE:
						program->sketch->has_p1 = false;
						program->mode = MODE_IN_SKETCH;
						program->show_hover_point = false;
						break;
					case MODE_IN_SKETCH:
						deselect_all(program->sketch);
						break;
				}
			break;
		case GLFW_KEY_LEFT_SHIFT:
			program->keys.lshift = (action == GLFW_PRESS) ? KEY_DOWN : KEY_UP;
			break;
		case GLFW_KEY_BACKSPACE:
			if (action == GLFW_PRESS && program->mode == MODE_IN_SKETCH) {
				delete_selected_listener(NULL);
			}
			break;
		case GLFW_KEY_RIGHT_SHIFT:
			program->keys.rshift = (action == GLFW_PRESS) ? KEY_DOWN : KEY_UP;
			break;
		case GLFW_KEY_F:
			if (action == GLFW_PRESS && program->selected != NULL) {
				program->flags.focus_animating = true;
			}
			break;
		case GLFW_KEY_A:
			if (action == GLFW_PRESS) {
				toggle_anti_aliasing();
			}
			break;
		case GLFW_KEY_R:
			if (action == GLFW_PRESS) {
				reset_view_hard(program);
			}
			break;
	}
}

void framebuffer_size_callback(GLFWwindow* window, i32_t width, i32_t height)
{
	// make sure the viewport matches the new window dimensions; note that width and
	// height will be significantly larger than specified on retina displays.
	//printf("Width: %i, Height: %i\n", width, height);
	program->screen_height = height;
	program->screen_width = width;

	free(program->font_proj);
	program->font_proj = M4_orthographic(0.0, program->screen_height, 0.0, program->screen_width);

	glViewport(0, 0, program->screen_width, program->screen_height);
}

v3f_t raycast_from_mouse(plane_t *plane)
{
	return V3_raycast_plane(plane->normal, plane->pos, inverse(V2_to_V3(mouse_pos(), 0.0)), inverse(V3(0.0, 0.0, -1.0)));
}

bool left_mouse_clicked(program_state_t *p)
{
	return !glfwGetMouseButton(p->window, GLFW_MOUSE_BUTTON_LEFT) && program->mouse.state_left;
}
bool right_mouse_clicked(program_state_t *p)
{
	return !glfwGetMouseButton(p->window, GLFW_MOUSE_BUTTON_RIGHT) && program->mouse.state_right;
}
void mouse_button_callback(GLFWwindow* window, i32_t button, i32_t action, i32_t mods)
{
	f32_t *v, *p;
	f32_t *v_inv;
	v3f_t pos;
	menu_mouse_button_update(program, button, action, mods);

	if (menu_contains(program, V2(((program->mouse.x+1)/2) * program->screen_width, ((program->mouse.y+1)/2) * program->screen_height)))
		goto set_mouse_states;

	if ((program->mode != MODE_DRAWING_LINE && program->mode != MODE_IN_SKETCH) && left_mouse_clicked(program)) {
		if (program->hovered != NULL && program->hovered != program->selected)
			select(program);
		else if (program->selected != NULL)
			deselect(program);
	} else if (program->mode == MODE_DRAWING_LINE && left_mouse_clicked(program)) {

		pos = inverse_project(sketch_normal(program->sketch), mouse_pos());
		if ((program->keys.lshift == KEY_UP && program->keys.rshift == KEY_UP))
			try_snap(program, &pos);
		sketch_add(program->sketch, pos);

	} else if (program->mode == MODE_DRAWING_LINE && right_mouse_clicked(program)) {

		program->sketch->has_p1 = false;
		program->mode = MODE_IN_SKETCH;
		program->show_hover_point = false;

	} else if (program->mode == MODE_IN_SKETCH && left_mouse_clicked(program)) {
		sketch_select(program->sketch, inverse_project(sketch_normal(program->sketch), mouse_pos()), program->snap_dist);
	}

	if (program->mode == MODE_IN_EXTRUDE && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) && program->extruder->hovered) {
		program->extruder->pressed = true;
	} else if (program->mode == MODE_IN_EXTRUDE && !glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)){
		program->extruder->pressed = false;
	}

set_mouse_states:
	program->mouse.state_left	= glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	program->mouse.state_right	= glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
}

void mouse_dragged(GLFWwindow* window, i32_t button, v2f_t pos)
{
	f32_t x_vel;
	f32_t y_vel;

	if (button == GLFW_MOUSE_BUTTON_RIGHT && (program->keys.lshift == KEY_DOWN || program->keys.rshift == KEY_DOWN)) {
		x_vel = (pos.x - program->mouse.prev_x);
		y_vel = (program->mouse.prev_y - pos.y);
		M4_translate(&program->translation, V3(x_vel, -y_vel, 0.0));
	} else if ((program->mode != MODE_IN_SKETCH && program->mode != MODE_DRAWING_LINE) && button == GLFW_MOUSE_BUTTON_RIGHT) {
		x_vel = (pos.x - program->mouse.prev_x);
		y_vel = (program->mouse.prev_y - pos.y);
		M4_rotate(&program->rotation, V3(0.0, 1.0, 0.0), x_vel);
		M4_rotate(&program->rotation, V3(1.0, 0.0, 0.0), y_vel);
	}
}

v3f_t ortho_proj(v3f_t n, v3f_t p, v3f_t pos)
{
	return V3_raycast_plane(n, p, pos, V3(0.0, 0.0, 1.0));
}



void mouse_motion_callback(GLFWwindow* window, f64_t x_in, f64_t y_in)
{
	v3f_t p_proj;

	update_mouse_pos(program, x_in, y_in);
	menu_mouse_motion_update(program, x_in, program->screen_height - y_in);
	if (menu_contains(program, V2(((program->mouse.x+1)/2) * program->screen_width, 
		((program->mouse.y+1)/2) * program->screen_height))) {
		unhover(program);
		if (program->extruder != NULL)
			program->extruder->pressed = false;
		return;
	}

	if (!is_sketching(program)) {
		//glfwSetCursor(program->window, program->cursor_crosshair);
		update_hovered(program, inverse(V2_to_V3(mouse_pos(), 0.0)));

	} else if (program->mode == MODE_DRAWING_LINE) {
		p_proj = inverse_project(sketch_normal(program->sketch), mouse_pos());
		if (program->keys.lshift != KEY_DOWN && program->keys.rshift != KEY_DOWN)
			try_snap(program, &p_proj);
		program->hover_point = p_proj;
		program->hover_point_normal = sketch_normal(program->sketch);
		program->has_hover_point = true;
		program->show_hover_point = true;
	}

	if (program->mouse.state_left)
		mouse_dragged(window, GLFW_MOUSE_BUTTON_LEFT, mouse_pos());
	if (program->mouse.state_right)
		mouse_dragged(window, GLFW_MOUSE_BUTTON_RIGHT,  mouse_pos());

	//printf("button: %i\n", GUI_component_contains(GLOBL_GUI.button1, V2(x_in, y_in)));
}

void scroll_callback(GLFWwindow* window, f64_t x_off, f64_t y_off)
{
	if (program->projection_type == PERSPECTIVE_PROJECTION)
		M4_translate(&program->translation, V3(0.0, 0.0, y_off*0.1));
	else
		M4_scale(&program->scale, V3(1 + y_off*0.1, 1 + y_off*0.1, 1 + y_off*0.1));
}

f32_t delta_time()
{
	f32_t current_time;
	static f32_t delta_time = 0.0f;
	static f32_t last_frame = 0.0f;

	current_time = glfwGetTime();

	delta_time = current_time - last_frame;
	last_frame = current_time;

	return delta_time;
}


void update_fps(f32_t rate)
{
	static f32_t timer = 0;
	static f32_t fps = 0;

	if (glfwGetTime() >= timer) {
		timer += rate;
		GLOBL_FPS = 1.0 / GLOBL_DELTA_TIME;
	}
}


void animate_focus_plane(f32_t delay_, u32_t _steps)
{
	static f32_t timer = 0;
	static u32_t steps = 0;
	static f32_t *current = NULL;
	static f32_t *new = NULL;
	static f32_t *current_trans = NULL;
	static f32_t *new_trans = NULL;
	static f32_t delay;
	f32_t step;

	if (steps == _steps) {
		timer = 0;
		steps = 0;
		program->flags.focus_animating = false;
		free(current);
		free(new);
		free(current_trans);
		free(new_trans);
		current		= NULL;
		new		= NULL;
		current_trans	= NULL;
		new_trans	= NULL;
		program->selected->selected = false;
		program->selected = NULL;
		delay = 0;
		return;
	}

	if (steps == 0) {
		new = rotational_transform(get_normal(*program->selected),V3(0.0, 0.0, 1.0));
		delay = delay_;
		current = program->rotation;
		current_trans = program->translation;
		new_trans = M4_eye();

		timer = glfwGetTime() + delay;
		steps++;
		step = (1.0 / _steps) * steps;
		program->rotation = M4_lerp(current, new, step);
		program->translation = M4_lerp(current_trans, new_trans, step);
		return;
	}

	if (glfwGetTime() >= timer) {
		timer += delay;
		free(program->rotation);
		steps++;
		step = (1.0 / _steps) * steps;
		program->rotation = M4_lerp(current, new, step);
		program->translation = M4_lerp(current_trans, new_trans, step);
	}
}

void draw_fps()
{
	update_fps(0.3);
	render_textf(program->font,
		(program->screen_width) - 100.0f, (program->screen_height) - 30.0f,
		0.3f, V3(1.0f, 1.0f, 1.0f), program->font_proj, "FPS: %.1f", GLOBL_FPS);
}

void main_loop()
{
	v3f_t cursor_pos;
	f32_t *proj_;
	f32_t *view_;
	glfwSetCursor(program->window, program->cursor_crosshair);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	while (!glfwWindowShouldClose(program->window)) {

		GLOBL_DELTA_TIME = delta_time();

		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		if (program->flags.focus_animating)
			animate_focus_plane(0.001, 10);

		view_ = view(program);
		proj_ = projection(program);

		if (is_sketching(program))
			show_sketch(program->sketch, program->plane_shader, program->font, view_, proj_, program->hover_point);

		FOREACH_DLA(program->content, i, selectable_t, s, {
			draw_selectable(program, s, view_, proj_);
		})

		if (program->show_hover_point && program->has_hover_point) {
			draw_dot_normal(program->hover_point,program->hover_point_normal, 0.01, V4(1.0,1.0,1.0,1.0), view_, proj_);
		}

		if (program->mode == MODE_IN_EXTRUDE) {
			render_extruder(program->extruder, program->extruder_shader, program->plane_shader, program->rotation, view_, proj_, true);
		}
		menu_draw(program);
		free(view_);
		free(proj_);

		draw_fps();
		glfwSwapBuffers(program->window);
		glfwPollEvents();
	}
}

void init_window(GLFWwindow** window, const char * title, u32_t width,
		u32_t height, void (*fsc)(GLFWwindow*, i32_t, i32_t))
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GL_TRUE);
#endif
	*window = glfwCreateWindow(width, height, title, NULL, NULL);
	if (window == NULL) {
		printf("Failed to create GLFW window\n");
		glfwTerminate();
		exit(-1);
	}
	glfwMakeContextCurrent(*window);

	/* set calback functions for window event handling */
	glfwSetFramebufferSizeCallback(*window, fsc);
	glfwSetKeyCallback(*window, keypress_callback);
	glfwSetCursorPosCallback(*window, mouse_motion_callback);
	glfwSetMouseButtonCallback(*window, mouse_button_callback);
	glfwSetScrollCallback(*window, scroll_callback);
	glfwSetCharCallback(*window, text_input_callback);
	//glfwSetInputMode(*window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	//glfwSetInputMode(*window, GLFW_STICKY_KEYS, GLFW_TRUE);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		printf("Failed to initialize GLAD\n");
		exit(-1);
	}
}


i32_t main(i32_t argc, char * argv[])
{
	GLFWwindow* window;


	init_window(&window, "CAD",
		START_WIDTH, START_HEIGHT, framebuffer_size_callback);
	init_program(window, START_WIDTH, START_HEIGHT, 45.0);
	main_loop();

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}