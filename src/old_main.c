/*==========================================================*
 *
 * @author Gustaf Franzén :: https://github.com/BjorneEk;
 *
 * a simple CAD program
 *==========================================================*/


// #include "sketch.h"
// #include "util/types.h"
// #include "util/error.h"
// #include "GL/shader.h"
// #include "GL/font.h"
// #include "matrix/matrix.h"
// #include "matrix/vector.h"
// #include "structures/dynamic_array.h"
// #include "plane.h"
// #include "GUI/GUI.h"

// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <math.h>
// #include <time.h>
// #include <glad/glad.h>
// #include <GLFW/glfw3.h>


// // global variables

// static GLFWwindow* GLOBL_WINDOW;
// static u32_t	GLOBL_SCR_WIDTH  = 1920;
// static u32_t	GLOBL_SCR_HEIGHT = 1080;
// _Atomic f32_t	GLOBL_DELTA_TIME = 0;
// static f32_t	GLOBL_FPS = 0;

// static const f32_t GLOBL_FOV = 45.0f;

// static font_t		*GLOBL_FONT;
// static f32_t		*GLOBL_FONT_PROJ;
// static GLFWcursor	*GLOBL_CROSSHAIR_CURSOR;
// static GLFWcursor	*GLOBL_HAND_CURSOR;

// static f32_t *GLOBL_PERSPECTIVE;
// static f32_t *GLOBL_PERSPECTIVE_INV;
// static f32_t *GLOBL_ORTHOGRAPHIC;
// static f32_t *GLOBL_ORTHOGRAPHIC_INV;

// static int PROJECTION_TYPE;

// static bool GLOBL_ANTI_ALIASING_ENABLED;

// enum {
// 	ORTHOGRAPHIC,
// 	PERSPECTIVE
// };

// static f32_t *GLOBL_VIEW_ROTATION;
// static f32_t *GLOBL_VIEW_TRANSLATION;
// static f32_t *GLOBL_INDIRECT_VIEW_;

// static f32_t *GLOBL_PROJ;
// static f32_t *GLOBL_PROJ_INV;

// static shader_t	GLOBL_PLANE_SHADER;
// static dla_t	*planes;
// static i32_t	SELECTED_PLANE_IDX;
// static dla_t	*axes;

// static f32_t SNAP_DIST = 0.01;
// bool focus_animating = false;

// struct {
// 	sketch_t *scetch;
// 	plane_t *plane;
// 	bool in_scetch;

// 	bool drawing_line;
// 	line_t line;
// } state = {
// 	NULL, NULL, false, false,
// };

// enum key_state {
// 	DOWN,
// 	UP
// };

// static struct {
// 	i32_t lshift;
// 	i32_t rshift;
// } GLOBL_KEYS = {
// 	UP, UP
// };

// static struct {
// 	f32_t x;
// 	f32_t y;
// 	f32_t prev_x;
// 	f32_t prev_y;
// 	i32_t state_left;
// 	i32_t state_right;
// 	bool first;
// } GLOBL_MOUSE = {
// 	0, 0, 0,
// 	0, 0, 0,
// 	true
// };

// struct {
// 	GUI_ctx *ctx;

// 	GUI_palette *regular;
// 	GUI_palette *focused;
// 	GUI_palette *hovered;
// 	GUI_palette *pressed;
// 	GUI_palette *toggled;

// 	GUI_component *sketch_btn;
// 	GUI_component *projection_tgl;

// } GLOBL_GUI;

// f32_t			*view();
// f64_t			rad(f64_t deg);
// static inline v2f_t	mouse_pos();
// void			init_globals();
// f32_t			delta_time();
// void			reset_viewproj();
// v3f_t		inverse_project(plane_t *p, f32_t x, f32_t y);
// void		mouse_button_callback(GLFWwindow* window, i32_t button, i32_t action, i32_t mods);
// void		mouse_motion_callback(GLFWwindow* window, f64_t x_in, f64_t y_in);
// void		scroll_callback(GLFWwindow* window, f64_t x_in, f64_t y_in);
// void		framebuffer_size_callback(GLFWwindow* window, i32_t width, i32_t height);
// void		mouse_dragged(GLFWwindow* window, i32_t button, f64_t x_in, f64_t y_in);
// void		update_fps(f32_t rate);
// void		add_orgin_axes();
// void		add_orgin_planes();
// void		main_loop();
// GLFWcursor*	create_crosshair_cursor(u32_t size);
// void		init_window(GLFWwindow** window, const char * title, u32_t width,
// 		u32_t height, void (*fsc)(GLFWwindow*, i32_t, i32_t));




// f32_t *view()
// {
// 	M4_clear(GLOBL_INDIRECT_VIEW_);
// 	M4_set_diagonal(GLOBL_INDIRECT_VIEW_, 1.0, 1.0, 1.0, 1.0);
// 	M4_mult(GLOBL_INDIRECT_VIEW_, GLOBL_VIEW_TRANSLATION);
// 	M4_mult(GLOBL_INDIRECT_VIEW_, GLOBL_VIEW_ROTATION);
// 	return GLOBL_INDIRECT_VIEW_;
// }

// f32_t *proj()
// {
// 	if (PROJECTION_TYPE == PERSPECTIVE) {
// 		GLOBL_PROJ = GLOBL_PERSPECTIVE;
// 	} else {
// 		GLOBL_PROJ = GLOBL_ORTHOGRAPHIC;
// 	}
// 	return GLOBL_PROJ;
// }

// f32_t *inv_proj()
// {
// 	if (PROJECTION_TYPE == PERSPECTIVE) {
// 		GLOBL_PROJ_INV = GLOBL_PERSPECTIVE_INV;
// 	} else {
// 		GLOBL_PROJ_INV = GLOBL_ORTHOGRAPHIC_INV;
// 	}
// 	return GLOBL_PROJ_INV;
// }

// f32_t *inv_viewproj()
// {
// 	f32_t *view_inv;
// 	f32_t *test;

// 	//test = M4_eye();
// 	//view_inv = M4_invert_model(GLOBL_VIEW_ROTATION, test);
// 	//free(test);
// 	view_inv = M4_invert_model(GLOBL_VIEW_ROTATION, GLOBL_VIEW_TRANSLATION);
// 	M4_mult(view_inv, inv_proj());
// 	return view_inv;
// }

// v3f_t project(plane_t *p, v3f_t u)
// {
// 	v3f_t u_;

// 	u_ = V3_scale(p->normal, V3_dot(p->normal, u));

// 	return V3_sub(u, u_);
// }

// v3f_t inverse_project(plane_t *p, f32_t x, f32_t y)
// {
// 	f32_t *inv;
// 	v3f_t res;

// 	inv = inv_viewproj();
// 	//inv = M4_invert_model(GLOBL_VIEW_ROTATION, GLOBL_VIEW_TRANSLATION);
// 	res = M4_mult_V3(inv, V3(x, y, 1.0));

// 	free(inv);
// 	return project(p, res);
// }

// f32_t *viewproj()
// {
// 	f32_t *proj;
// 	proj = M4_copy(GLOBL_PROJ);
// 	M4_mult(proj, view());
// 	return proj;
// }

// f64_t	rad(f64_t deg)
// {
// 	const f64_t rads_per_deg = (M_PI / 180.0);
// 	return deg * rads_per_deg;
// }

// static inline v2f_t mouse_pos()
// {
// 	return V2(GLOBL_MOUSE.x, GLOBL_MOUSE.y);
// }

// f32_t *default_rot()
// {
// 	return M4_eye();
// }

// f32_t *default_trans()
// {
// 	f32_t *res;
// 	res = M4_eye();
// 	if (PROJECTION_TYPE == PERSPECTIVE) {
// 		res[13] = -0.4;
// 		res[14] = -3.0;
// 	}
// 	return res;
// }

// void toggle_anti_aliasing()
// {
// 	if (!GLOBL_ANTI_ALIASING_ENABLED) {
// 		//glfwWindowHint(GLFW_SAMPLES, 4);
// 		glEnable(GL_MULTISAMPLE);
// 		GLOBL_ANTI_ALIASING_ENABLED = true;
// 	} else {
// 		//glfwWindowHint(GLFW_SAMPLES, 1);
// 		glDisable(GL_MULTISAMPLE);
// 		GLOBL_ANTI_ALIASING_ENABLED = false;
// 	}
// }





// void toggle_proj_type_listener(GUI_component *c)
// {
// 	PROJECTION_TYPE = PROJECTION_TYPE == PERSPECTIVE ? ORTHOGRAPHIC : PERSPECTIVE;
// 	reset_viewproj();
// }

// void hand_cursor_listener(GUI_component *c)
// {
// 	glfwSetCursor(GLOBL_WINDOW, GLOBL_HAND_CURSOR);
// }
// void crosshair_cursor_listener(GUI_component *c)
// {
// 	glfwSetCursor(GLOBL_WINDOW, GLOBL_CROSSHAIR_CURSOR);
// }


// void init_GUI()
// {
// 	GLOBL_GUI.ctx = GUI_new(V2(0, GLOBL_SCR_HEIGHT *0.8),GLOBL_SCR_WIDTH, GLOBL_SCR_HEIGHT*0.2, V4(1.0,1.0,1.0,0.5), GLOBL_SCR_WIDTH, GLOBL_SCR_HEIGHT);

// 	GLOBL_GUI.regular = GUI_new_palette(V4(0.8,0.8,0.8,1.0), V4(0.0,0.0,0.0,1.0));
// 	GLOBL_GUI.focused = GUI_new_palette(V4(0.8,0.8,0.8,1.0), V4(0.5,0.5,0.0,1.0));
// 	GLOBL_GUI.hovered = GUI_new_palette(V4(0.7,0.7,0.7,1.0), V4(0.0,0.0,0.0,1.0));
// 	GLOBL_GUI.pressed = GUI_new_palette(V4(0.5,0.5,0.5,1.0), V4(0.0,0.0,0.0,1.0));
// 	GLOBL_GUI.toggled = GUI_new_palette(V4(0.8,0.8,0.8,1.0), V4(0.0,0.0,0.0,1.0));
// 	GLOBL_GUI.sketch_btn = GUI_button(
// 		20, GLOBL_SCR_HEIGHT - 85,
// 		115, 30, 5, GUI_LINE_BORDER,
// 		GLOBL_GUI.regular, GLOBL_GUI.focused, GLOBL_GUI.hovered,
// 		GLOBL_GUI.pressed, GLOBL_GUI.toggled, GLOBL_FONT, "Scetch");
// 	GLOBL_GUI.projection_tgl = GUI_toggle_button(
// 		210, GLOBL_SCR_HEIGHT - 50,
// 		70, 30, 15, GUI_LINE_BORDER,
// 		GLOBL_GUI.regular, GLOBL_GUI.focused, GLOBL_GUI.hovered,
// 		GLOBL_GUI.pressed, GLOBL_GUI.toggled,
// 		V4(1.0, 0.0, 0.0, 1.0), V4(0.0, 1.0, 0.0, 1.0), GLOBL_FONT, "Perspective", "Orthographic");

// 	GUI_add_listener(GLOBL_GUI.projection_tgl, GUI_TOGGLED, toggle_proj_type_listener);
// 	GUI_add_listener(GLOBL_GUI.sketch_btn, GUI_HOVERED, hand_cursor_listener);
// 	GUI_add_listener(GLOBL_GUI.sketch_btn, GUI_UNHOVERED, crosshair_cursor_listener);
// 	GUI_add_listener(GLOBL_GUI.projection_tgl, GUI_HOVERED, hand_cursor_listener);
// 	GUI_add_listener(GLOBL_GUI.projection_tgl, GUI_UNHOVERED, crosshair_cursor_listener);

// 	GUI_add(GLOBL_GUI.ctx, GLOBL_GUI.sketch_btn);
// 	GUI_add(GLOBL_GUI.ctx, GLOBL_GUI.projection_tgl);

// }

// void destroy_GUI()
// {
// 	GUI_destroy(&GLOBL_GUI.ctx);
// 	GUI_destroy_component(&GLOBL_GUI.projection_tgl);
// 	GUI_destroy_component(&GLOBL_GUI.sketch_btn);
// 	free(GLOBL_GUI.regular);
// 	free(GLOBL_GUI.focused);
// 	free(GLOBL_GUI.hovered);
// 	free(GLOBL_GUI.pressed);
// 	free(GLOBL_GUI.toggled);
// }

// void init_globals()
// {

// 	GLOBL_FONT = new_font("/System/Library/Fonts/Monaco.ttf");
// 	GLOBL_FONT_PROJ = M4_orthographic(0.0, GLOBL_SCR_HEIGHT, 0.0, GLOBL_SCR_WIDTH);

// 	PROJECTION_TYPE = PERSPECTIVE;

// 	GLOBL_VIEW_ROTATION	= default_rot();
// 	GLOBL_VIEW_TRANSLATION	= default_trans();
// 	GLOBL_INDIRECT_VIEW_	= M4_eye();

// 	GLOBL_PERSPECTIVE = M4_perspective(rad(GLOBL_FOV), (f32_t)GLOBL_SCR_WIDTH/(f32_t)GLOBL_SCR_HEIGHT, 0.1, 100.0);
// 	GLOBL_PERSPECTIVE_INV = M4_inverse_perspective(rad(GLOBL_FOV), (f32_t)GLOBL_SCR_WIDTH/(f32_t)GLOBL_SCR_HEIGHT, 0.1, 100.0);

// 	GLOBL_ORTHOGRAPHIC = M4_orthographic(-1.0*GLOBL_SCR_HEIGHT/GLOBL_SCR_WIDTH, 1.0*GLOBL_SCR_HEIGHT/GLOBL_SCR_WIDTH, -1.0, 1.0);
// 	GLOBL_ORTHOGRAPHIC_INV = M4_inverse_orthographic(-1.0*GLOBL_SCR_HEIGHT/GLOBL_SCR_WIDTH, 1.0*GLOBL_SCR_HEIGHT/GLOBL_SCR_WIDTH, -1.0, 1.0);


// 	proj();
// 	inv_proj();
// 	shader_load(&GLOBL_PLANE_SHADER, "src/shaders/plane_vert.glsl", "src/shaders/plane_frag.glsl");
// 	planes = DLA_new(sizeof(plane_t*), 10);
// 	axes = DLA_new(sizeof(axis_t*), 10);
// 	GLOBL_CROSSHAIR_CURSOR = create_crosshair_cursor(17);
// 	GLOBL_HAND_CURSOR = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
// 	SELECTED_PLANE_IDX = -1;
// 	add_orgin_planes();
// 	add_orgin_axes();

// 	GLOBL_ANTI_ALIASING_ENABLED = false;

// 	init_GUI();
// }



// void reset_viewproj()
// {
// 	free(GLOBL_VIEW_ROTATION);
// 	GLOBL_VIEW_ROTATION = default_rot();
// 	free(GLOBL_VIEW_TRANSLATION);
// 	GLOBL_VIEW_TRANSLATION = default_trans();
// }

// void keypress_callback(GLFWwindow* window, i32_t key, i32_t scancode, i32_t action, i32_t mods)
// {
// 	f32_t *focus_view;
// 	plane_t *p;
// 	switch (key) {
// 		case GLFW_KEY_ESCAPE:
// 			if (action == GLFW_PRESS && state.in_scetch)
// 				state.in_scetch = false;
// 				return;
// 			break;
// 		case GLFW_KEY_LEFT_SHIFT:
// 			GLOBL_KEYS.lshift = (action == GLFW_PRESS) ? DOWN : UP;
// 			break;
// 		case GLFW_KEY_RIGHT_SHIFT:
// 			GLOBL_KEYS.rshift = (action == GLFW_PRESS) ? DOWN : UP;
// 			break;
// 		case GLFW_KEY_F:
// 			if (action == GLFW_PRESS && SELECTED_PLANE_IDX != -1) {
// 				focus_animating = true;
// 			}
// 			break;
// 		case GLFW_KEY_A:
// 			if (action == GLFW_PRESS) {
// 				toggle_anti_aliasing();
// 			}
// 			break;
// 		case GLFW_KEY_S:
// 			if (!state.in_scetch && action == GLFW_PRESS && SELECTED_PLANE_IDX != -1) {
// 				if (PROJECTION_TYPE == PERSPECTIVE)
// 					GUI_toggle(GLOBL_GUI.projection_tgl);
// 				GLOBL_GUI.projection_tgl->enabled = false;
// 				focus_animating = true;
// 				p = *(plane_t**)DLA_get(planes, SELECTED_PLANE_IDX);
// 				state.scetch = begin_sketch(p);
// 				state.plane = p;
// 				state.in_scetch = true;
// 			} else if (state.in_scetch && action == GLFW_PRESS) {
// 				GLOBL_GUI.projection_tgl->enabled = true;
// 				state.in_scetch = false;
// 			}
// 			break;
// 		case GLFW_KEY_R:
// 			if (action == GLFW_PRESS) {
// 				reset_viewproj();
// 			}
// 			break;
// 	}
// }

// void framebuffer_size_callback(GLFWwindow* window, i32_t width, i32_t height)
// {
// 	// make sure the viewport matches the new window dimensions; note that width and
// 	// height will be significantly larger than specified on retina displays.
// 	GLOBL_SCR_HEIGHT = height;
// 	GLOBL_SCR_WIDTH = width;

// 	free(GLOBL_FONT_PROJ);
// 	GLOBL_FONT_PROJ = M4_orthographic(0.0, GLOBL_SCR_HEIGHT, 0.0, GLOBL_SCR_WIDTH);

// 	glViewport(0, 0, GLOBL_SCR_WIDTH, GLOBL_SCR_HEIGHT);
// }

// void mouse_button_callback(GLFWwindow* window, i32_t button, i32_t action, i32_t mods)
// {
// 	f32_t *v;
// 	f32_t *v_inv;
// 	v3f_t pos;
// 	GUI_mouse_button_update(GLOBL_GUI.ctx, button, action, mods);
// 	if (GUI_contains(GLOBL_GUI.ctx, V2(((GLOBL_MOUSE.x+1)/2) * GLOBL_SCR_WIDTH, ((GLOBL_MOUSE.y+1)/2) * GLOBL_SCR_HEIGHT)))
// 		return;
// 	/* this means the button has been pressed and is now released */
// 	if (!state.in_scetch && !glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) && GLOBL_MOUSE.state_left) {
// 		SELECTED_PLANE_IDX = -1;
// 		v = view();
// 		FOREACH_DLA(planes, i, plane_t*, p, {
// 			if (SELECTED_PLANE_IDX == -1 && is_plane(p, v, proj(), mouse_pos())) {
// 				SELECTED_PLANE_IDX = i;
// 				p->highlighted = true;
// 			} else {
// 				state.plane = NULL;
// 				p->highlighted = false;
// 			}
// 		});
// 	} else if (state.in_scetch && !state.drawing_line &&
// 		!glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) && GLOBL_MOUSE.state_left) { //start drawing a line segment

// 		state.drawing_line = true;
// 		pos = inverse_project(state.plane, GLOBL_MOUSE.x, GLOBL_MOUSE.y);
// 			if ((GLOBL_KEYS.lshift == UP && GLOBL_KEYS.rshift == UP))
// 				try_snap(state.scetch, &pos, SNAP_DIST);
// 		state.line.p1 = pos;

// 	} else if (state.in_scetch && state.drawing_line &&
// 		!glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) && GLOBL_MOUSE.state_left) { //add line to scetch and continue segment

// 		pos = inverse_project(state.plane, GLOBL_MOUSE.x, GLOBL_MOUSE.y);
// 			if ((GLOBL_KEYS.lshift == UP && GLOBL_KEYS.rshift == UP))
// 				try_snap(state.scetch, &pos, SNAP_DIST);

// 		state.line.p2 = pos;
// 		add_line(state.scetch, state.line);
// 		state.line.p1 = state.line.p2;

// 	} else if (state.in_scetch && state.drawing_line &&
// 		!glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) && GLOBL_MOUSE.state_right) { // end segment

// 		state.drawing_line = false;

// 	}
// 	//printf("button: %i, action: %i\n", button, action);
// 	GLOBL_MOUSE.state_left	= glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
// 	GLOBL_MOUSE.state_right	= glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
// }

// void mouse_dragged(GLFWwindow* window, i32_t button, f64_t x_in, f64_t y_in)
// {
// 	f32_t x_vel;
// 	f32_t y_vel;

// 	if (button == GLFW_MOUSE_BUTTON_RIGHT && (GLOBL_KEYS.lshift == DOWN || GLOBL_KEYS.rshift == DOWN)) {
// 		x_vel = (x_in - GLOBL_MOUSE.prev_x);
// 		y_vel = (GLOBL_MOUSE.prev_y - y_in);
// 		M4_translate(&GLOBL_VIEW_TRANSLATION, V3(x_vel, -y_vel, 0.0));
// 	} else if (!state.in_scetch && button == GLFW_MOUSE_BUTTON_RIGHT) {
// 		x_vel = (x_in - GLOBL_MOUSE.prev_x);
// 		y_vel = (GLOBL_MOUSE.prev_y - y_in);
// 		M4_rotate(&GLOBL_VIEW_ROTATION, V3(0.0, 1.0, 0.0), x_vel);
// 		M4_rotate(&GLOBL_VIEW_ROTATION, V3(1.0, 0.0, 0.0), y_vel);
// 	}
// }

// void mouse_motion_callback(GLFWwindow* window, f64_t x_in, f64_t y_in)
// {
// 	f32_t x;
// 	f32_t y;



// 	x = (x_in / GLOBL_SCR_WIDTH) * 2 - 1;
// 	y = -((y_in / GLOBL_SCR_HEIGHT) * 2 - 1);

// 	GUI_mouse_motion_callback(GLOBL_GUI.ctx, x_in, GLOBL_SCR_HEIGHT - y_in);

// 	if (GLOBL_MOUSE.first) {
// 		GLOBL_MOUSE.x = x;
// 		GLOBL_MOUSE.y = y;
// 		GLOBL_MOUSE.first = false;
// 	}

// 	GLOBL_MOUSE.prev_x = GLOBL_MOUSE.x;
// 	GLOBL_MOUSE.x = x;
// 	GLOBL_MOUSE.prev_y = GLOBL_MOUSE.y;
// 	GLOBL_MOUSE.y = y;

// 	if (GLOBL_MOUSE.state_left)
// 		mouse_dragged(window, GLFW_MOUSE_BUTTON_LEFT, x, y);
// 	if (GLOBL_MOUSE.state_right)
// 		mouse_dragged(window, GLFW_MOUSE_BUTTON_RIGHT, x, y);

// 	//printf("button: %i\n", GUI_component_contains(GLOBL_GUI.button1, V2(x_in, y_in)));
// }

// void scroll_callback(GLFWwindow* window, f64_t x_off, f64_t y_off)
// {
// 	M4_translate(&GLOBL_VIEW_TRANSLATION, V3(0.0, 0.0, y_off*0.1));
// }

// f32_t delta_time()
// {
// 	f32_t current_time;
// 	static f32_t delta_time = 0.0f;
// 	static f32_t last_frame = 0.0f;

// 	current_time = glfwGetTime();

// 	delta_time = current_time - last_frame;
// 	last_frame = current_time;

// 	return delta_time;
// }

// void add_orgin_planes()
// {
// 	plane_t *tmp;

// 	tmp = new_plane(V3(0.0, 0.0, 1.0), V3(1.0, 0.0, 0.0), V3(0.0, 0.0, 0.0), 1, GLOBL_PLANE_SHADER);
// 	DLA_append(planes, &tmp);
// 	tmp = new_plane(V3(0.0, 1.0, 0.0),  V3(1.0, 0.0, 0.0), V3(0.0, 0.0, 0.0), 1, GLOBL_PLANE_SHADER);
// 	DLA_append(planes, &tmp);
// 	tmp = new_plane(V3(1.0, 0.0, 0.0),  V3(0.0, 1.0, 0.0), V3(0.0, 0.0, 0.0), 1, GLOBL_PLANE_SHADER);
// 	DLA_append(planes, &tmp);
// }

// void add_orgin_axes()
// {
// 	axis_t *tmp;

// 	tmp = new_axis(V3(1.0, 0.0, 0.0), V3(0.0, 0.0, 0.0), 1.8, GLOBL_PLANE_SHADER);
// 	DLA_append(axes, &tmp);
// 	tmp = new_axis(V3(0.0, 1.0, 0.0), V3(0.0, 0.0, 0.0), 1.8, GLOBL_PLANE_SHADER);
// 	DLA_append(axes, &tmp);
// 	tmp = new_axis(V3(0.0, 0.0, 1.0), V3(0.0, 0.0, 0.0), 1.8, GLOBL_PLANE_SHADER);
// 	DLA_append(axes, &tmp);
// }

// void update_fps(f32_t rate)
// {
// 	static f32_t timer = 0;
// 	static f32_t fps = 0;

// 	if (glfwGetTime() >= timer) {
// 		timer += rate;
// 		GLOBL_FPS = 1.0 / GLOBL_DELTA_TIME;
// 	}
// }



// void animate_focus_plane(f32_t delay, u32_t _steps)
// {
// 	static f32_t timer = 0;
// 	static u32_t steps = 0;
// 	static f32_t *current = NULL;
// 	static f32_t *new = NULL;
// 	static f32_t *current_trans = NULL;
// 	static f32_t *new_trans = NULL;

// 	plane_t *p;
// 	f32_t step;

// 	if (steps == _steps) {
// 		timer = 0;
// 		steps = 0;
// 		focus_animating = false;
// 		free(current);
// 		free(new);
// 		free(current_trans);
// 		free(new_trans);
// 		current		= NULL;
// 		new		= NULL;
// 		current_trans	= NULL;
// 		new_trans	= NULL;
// 		SELECTED_PLANE_IDX = -1;
// 		return;
// 	}

// 	if (steps == 0) {
// 		p = *(plane_t**)DLA_get(planes, SELECTED_PLANE_IDX);
// 		new = look_at_plane(p);
// 		current = GLOBL_VIEW_ROTATION;
// 		current_trans = GLOBL_VIEW_TRANSLATION;
// 		new_trans = default_trans();
// 		p->highlighted = false;
// 	}

// 	if (glfwGetTime() >= timer) {
// 		timer += delay;

// 		if (steps != 0)
// 			free(GLOBL_VIEW_ROTATION);
// 		steps++;
// 		step = (1.0 / _steps) * steps;
// 		GLOBL_VIEW_ROTATION = M4_lerp(current, new, step);
// 		GLOBL_VIEW_TRANSLATION = M4_lerp(current_trans, new_trans, step);
// 	}
// }

// void main_loop()
// {
// 	v3f_t pos;

// 	glfwSetCursor(GLOBL_WINDOW, GLOBL_CROSSHAIR_CURSOR);

// 	glEnable(GL_DEPTH_TEST);
// 	glEnable(GL_MULTISAMPLE);
// 	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

// 	while (!glfwWindowShouldClose(GLOBL_WINDOW)) {

// 		GLOBL_DELTA_TIME = delta_time();

// 		glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
// 		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
// 		update_fps(0.3);
// 		render_textf(GLOBL_FONT, (GLOBL_SCR_WIDTH) - 100.0f, (GLOBL_SCR_HEIGHT) - 30.0f, 0.3f, V3(1.0f, 1.0f, 1.0f), GLOBL_FONT_PROJ, "FPS: %.1f", GLOBL_FPS);

// 		if (focus_animating)
// 			animate_focus_plane(1.0, 10);

// 		if (state.in_scetch) {
// 			if (state.drawing_line) {
// 				pos = inverse_project(state.plane, GLOBL_MOUSE.x, GLOBL_MOUSE.y);
// 				if ((GLOBL_KEYS.lshift == UP && GLOBL_KEYS.rshift == UP))
// 					try_snap(state.scetch, &pos, SNAP_DIST);
// 				draw_line(
// 					line(state.line.p1, pos),
// 					GLOBL_PLANE_SHADER,
// 					V3(1.0, 0.5, 0.0),
// 					view(),
// 					proj()
// 				);
// 			}
// 		}

// 		FOREACH_DLA(planes, i, plane_t*, p, {
// 			if (i == 0)
// 				show_plane(p, V3(1.0, 0.0, 0.0), view(), proj());
// 			else if (i == 1)
// 				show_plane(p, V3(0.0, 1.0, 0.0), view(), proj());
// 			else
// 				show_plane(p, V3(0.0, 0.0, 1.0), view(), proj());

// 			//show_plane(p, V3(1.0, 1.0, 0.0), view(), proj());
// 			if (p->sketch != NULL && p->sketch->lines->len > 0) {
// 				show_sketch(p->sketch, GLOBL_PLANE_SHADER, view(), proj());
// 			}
// 		});

// 		FOREACH_DLA(axes, i, axis_t*, a, {
// 			show_axis(a, V3(1.0, 1.0, 0.0), view(), proj());
// 		});
// 		GUI_draw(GLOBL_GUI.ctx);

// 		glfwSwapBuffers(GLOBL_WINDOW);
// 		glfwPollEvents();
// 	}
// }

// /**
//  * creates a pluss shaped crotthair cursor
//  * @size the width ot the cursor, this needs to be an uneaven number
//  **/
// GLFWcursor* create_crosshair_cursor(u32_t size)
// {
// 	u32_t *pixel_buffer;
// 	i32_t i;
// 	i32_t mid;

// 	pixel_buffer = calloc(size * size, sizeof(u32_t));

// 	mid = (size / 2);

// 	for (i = 0; i < size; i++) {
// 		pixel_buffer[mid + i * size] = 0xFF010101;
// 		pixel_buffer[mid * size + i] = 0xFF010101;
// 	}

// 	GLFWimage image;
// 	image.width = size;
// 	image.height = size;
// 	image.pixels = (u8_t*)pixel_buffer;

// 	return glfwCreateCursor(&image, mid, mid);
// }


// void init_window(GLFWwindow** window, const char * title, u32_t width,
// 		u32_t height, void (*fsc)(GLFWwindow*, i32_t, i32_t))
// {
// 	glfwInit();
// 	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
// 	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
// 	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
// 	glfwWindowHint(GLFW_SAMPLES, 4);

// #ifdef __APPLE__
// 	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
// #endif
// 	*window = glfwCreateWindow(width, height, title, NULL, NULL);
// 	if (window == NULL) {
// 		printf("Failed to create GLFW window\n");
// 		glfwTerminate();
// 		exit(-1);
// 	}
// 	glfwMakeContextCurrent(*window);

// 	/* set calback functions for window event handling */
// 	glfwSetFramebufferSizeCallback(*window, fsc);
// 	glfwSetKeyCallback(*window, keypress_callback);
// 	glfwSetCursorPosCallback(*window, mouse_motion_callback);
// 	glfwSetMouseButtonCallback(*window, mouse_button_callback);
// 	glfwSetScrollCallback(*window, scroll_callback);
// 	//glfwSetInputMode(*window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
// 	//glfwSetInputMode(*window, GLFW_STICKY_KEYS, GLFW_TRUE);

// 	// glad: load all OpenGL function pointers
// 	// ---------------------------------------
// 	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
// 		printf("Failed to initialize GLAD\n");
// 		exit(-1);
// 	}
// }


// i32_t main(i32_t argc, char * argv[])
// {

// 	init_window(&GLOBL_WINDOW, "OpenGL Window",
// 		GLOBL_SCR_WIDTH, GLOBL_SCR_HEIGHT, framebuffer_size_callback);
// 	init_globals();
// 	main_loop();
// 	destroy_GUI();
// 	// glfw: terminate, clearing all previously allocated GLFW resources.
// 	// ------------------------------------------------------------------
// 	glfwTerminate();
// 	return 0;
// }