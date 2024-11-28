/*==========================================================*
 *
 * @author Gustaf Franzén :: https://github.com/BjorneEk;
 *
 * Menu gui implementation
 *==========================================================*/

#include "GUI/GUI.h"
#include "GUI/GUI_component.h"
#include "program_state.h"
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>

bool	menu_contains(program_state_t *prog, v2f_t p)
{
	i32_t i;

	for (i = 0; i < prog->menu->ctx_count; i++) {
		if (prog->menu->contexts[i]->enabled && GUI_contains(prog->menu->contexts[i], p))
			return true;
	}
	return false;
}
void	menu_mouse_button_update(program_state_t *p, i32_t button, i32_t action, i32_t mods)
{
	i32_t i;
	for (i = 0; i < p->menu->ctx_count; i++) {
		p->menu->contexts[i]->pressed = action == GLFW_PRESS && p->menu->contexts[i]->hovered;
		if (p->menu->contexts[i]->enabled) {
			GUI_mouse_button_update(p->menu->contexts[i], button, action, mods);
		}
	}
}
void	menu_text_input_update(program_state_t *p, u32_t codepoint)
{
	i32_t i;

	for (i = 0; i < p->menu->ctx_count; i++) {
		if (p->menu->contexts[i]->enabled)
			GUI_character_update(p->menu->contexts[i], codepoint);
	}
}
void	menu_key_update(program_state_t *p, i32_t key, i32_t scancode, i32_t action, i32_t mods)
{
	i32_t i;

	for (i = 0; i < p->menu->ctx_count; i++) {
		if (p->menu->contexts[i]->enabled)
			GUI_key_update(p->menu->contexts[i], key, scancode, action, mods);
	}

}

void	menu_mouse_motion_update(program_state_t *p, f64_t x, f32_t y)
{
	i32_t i;
	for (i = 0; i < p->menu->ctx_count; i++) {
		if (p->menu->contexts[i]->enabled && GUI_contains(p->menu->contexts[i], V2(x, y)) && p->cursor_current != p->cursor_arrow) {
			glfwSetCursor(p->window, p->cursor_arrow);
			p->cursor_current = p->cursor_arrow;
		} else if (p->menu->contexts[i]->hovered && !GUI_contains(p->menu->contexts[i], V2(x, y))) {
			glfwSetCursor(p->window, p->cursor_crosshair);
			p->cursor_current = p->cursor_crosshair;
		}

		if (p->menu->contexts[i]->pressed && p->menu->contexts[i]->movable) {
			p->menu->contexts[i]->pos = V2_sub(p->menu->contexts[i]->pos, V2_sub(p->menu->contexts[i]->press_point, V2(x, y)));
		}
		if (GUI_contains(p->menu->contexts[i], V2(x, y)) && GUI_is_free(p->menu->contexts[i], V2(x, y))) {
			p->menu->contexts[i]->press_point = V2(x, y);
			p->menu->contexts[i]->hovered = true;
		} else {
			p->menu->contexts[i]->hovered = false;
		}


		if (p->menu->contexts[i]->enabled) {
			GUI_mouse_motion_update(p->menu->contexts[i], x, y);
		}
	}
}

void	menu_draw(program_state_t *prog)
{
	i32_t i;

	for (i = 0; i < prog->menu->ctx_count; i++) {
		GUI_draw(prog->menu->contexts[i]);
	}
}

void	init_menu(program_state_t *p)
{
	p->menu = malloc(sizeof(menu_t));
	p->menu->main = GUI_new(
		V2(0, p->screen_height * 0.8),			// pos
		p->screen_width, p->screen_height * 0.2,	// width, height
		V4(0.7,0.7,0.7,1.0),		// color
		p->screen_width, p->screen_height);		// screen size

	p->menu->sketch = GUI_new(
		V2(0, p->screen_height * 0.8),			// pos
		p->screen_width, p->screen_height * 0.2,	// width, height
		V4(0.8,1.0,1.0,0.5),				// color
		p->screen_width, p->screen_height);		// screen size
	p->menu->sketch->enabled = false;

	p->menu->sketch = GUI_new_rounded(
		V2(0, 0),					// pos
		400, 300,	// width, height
		10, 2,
		V4(0.7,0.7,0.7,1.0), V4(0.0,0.0,0.0,1.0),	// color
		p->screen_width, p->screen_height);		// screen size
	p->menu->sketch->movable = true;
	p->menu->sketch->enabled = false;
	p->menu->start_pos = V2(0, p->menu->main->pos.y - 300);
	p->menu->extrude = GUI_new_rounded(
		V2(0, 0),					// pos
		400, 300,	// width, height
		10, 2,
		V4(0.7,0.7,0.7,1.0), V4(0.0,0.0,0.0,1.0),	// color
		p->screen_width, p->screen_height);		// screen size
	p->menu->extrude->movable = true;
	p->menu->extrude->enabled = false;

	p->menu->regular = GUI_new_palette(V4(0.8,0.8,0.8,1.0), V4(0.0,0.0,0.0,1.0));
	p->menu->focused = GUI_new_palette(V4(0.8,0.8,0.8,1.0), V4(0.5,0.5,0.0,1.0));
	p->menu->hovered = GUI_new_palette(V4(0.7,0.7,0.7,1.0), V4(0.0,0.0,0.0,1.0));
	p->menu->pressed = GUI_new_palette(V4(0.5,0.5,0.5,1.0), V4(0.0,0.0,0.0,1.0));
	p->menu->toggled = GUI_new_palette(V4(0.8,0.8,0.8,1.0), V4(0.0,0.0,0.0,1.0));

	p->menu->regular_txt = GUI_new_palette(V4(1.0,1.0,1.0,1.0), V4(0.2,0.2,0.2,1.0));
	p->menu->focused_txt = GUI_new_palette(V4(1.0,1.0,1.0,1.0), V4(0.0,0.0,0.0,1.0));
	p->menu->hovered_txt = GUI_new_palette(V4(0.9,0.9,0.9,1.0), V4(0.0,0.0,0.0,1.0));
	p->menu->pressed_txt = GUI_new_palette(V4(0.8,0.8,0.8,1.0), V4(0.2,0.2,0.2,1.0));
	p->menu->toggled_txt = GUI_new_palette(V4(1.0,1.0,1.0,1.0), V4(0.2,0.2,0.2,1.0));

	p->menu->regular_red = GUI_new_palette(V4(0.7,0.2,0.2,1.0), V4(0.0,0.0,0.0,1.0));
	p->menu->focused_red = GUI_new_palette(V4(0.7,0.2,0.2,1.0), V4(0.0,0.0,0.0,1.0));
	p->menu->hovered_red = GUI_new_palette(V4(0.9,0.1,0.1,1.0), V4(0.9,0.1,0.1,1.0));
	p->menu->pressed_red = GUI_new_palette(V4(1.0,0.0,0.0,1.0), V4(0.0,0.0,0.0,1.0));
	p->menu->toggled_red = GUI_new_palette(V4(0.7,0.2,0.2,1.0), V4(0.0,0.0,0.0,1.0));

	p->menu->sketch_exit_btn = GUI_button(
		5, 275,
		20, 20, 10, GUI_LINE_BORDER,
		p->menu->regular_red, p->menu->focused_red, p->menu->hovered_red,
		p->menu->pressed_red, p->menu->toggled_red, p->font, "");
	p->menu->extrude_exit_btn = GUI_button(
		5, 275,
		20, 20, 10, GUI_LINE_BORDER,
		p->menu->regular_red, p->menu->focused_red, p->menu->hovered_red,
		p->menu->pressed_red, p->menu->toggled_red, p->font, "");

	p->menu->start_sketch_btn = GUI_button(
		20, 180,
		120, 30, 5, GUI_LINE_BORDER,
		p->menu->regular, p->menu->focused, p->menu->hovered,
		p->menu->pressed, p->menu->toggled, p->font, "Sketch");

	p->menu->extrude_btn = GUI_button(
		160, 180,
		120, 30, 5, GUI_LINE_BORDER,
		p->menu->regular, p->menu->focused, p->menu->hovered,
		p->menu->pressed, p->menu->toggled, p->font, "Extrude");

	p->menu->extrude_dist_lbl = GUI_label(
		30, 240,
		160, 30, 0, GUI_NO_BORDER,
		p->menu->hovered, p->font, "Distance:");


	p->menu->extrude_dist_text_field = GUI_text_field(
		190, 240,
		120, 30, 2, GUI_LINE_BORDER,
		p->menu->regular_txt, p->menu->focused_txt, p->menu->hovered_txt,
		p->menu->pressed_txt, p->menu->toggled_txt, p->font, "0.0");

	p->menu->extrude_select_btn = GUI_button(
		30, 200,
		220, 30, 2, GUI_LINE_BORDER,
		p->menu->regular, p->menu->focused, p->menu->hovered,
		p->menu->pressed, p->menu->toggled, p->font, "Select geometry");

	p->menu->extrude_finish_btn = GUI_button(
		30, 160,
		160, 30, 2, GUI_LINE_BORDER,
		p->menu->regular, p->menu->focused, p->menu->hovered,
		p->menu->pressed, p->menu->toggled, p->font, "Finish");

	p->menu->projection_tgl = GUI_toggle_button(
		1500, 170,
		70, 30, 15, GUI_LINE_BORDER,
		p->menu->regular, p->menu->focused, p->menu->hovered,
		p->menu->pressed, p->menu->toggled,
		V4(1.0, 0.0, 0.0, 1.0), V4(0.0, 1.0, 0.0, 1.0), p->font, "Perspective", "Orthographic");
	p->menu->show_edges_tgl = GUI_toggle_button(
		1500, 110,
		70, 30, 15, GUI_LINE_BORDER,
		p->menu->regular, p->menu->focused, p->menu->hovered,
		p->menu->pressed, p->menu->toggled,
		V4(1.0, 0.0, 0.0, 1.0), V4(0.0, 1.0, 0.0, 1.0), p->font, "Show", "Hide");

	p->menu->projection_tgl->handle_color_lerp = 1.0;
	p->menu->projection_tgl->handle_pos_lerp = 1.0;
	p->menu->projection_tgl->toggled = 1;

	p->menu->finish_sketch_btn = GUI_button(
		190, 200,
		150, 30, 5, GUI_LINE_BORDER,
		p->menu->regular, p->menu->focused, p->menu->hovered,
		p->menu->pressed, p->menu->toggled, p->font, "Finish");

	p->menu->line_btn = GUI_button(
		20, 200,
		150, 30, 5, GUI_LINE_BORDER,
		p->menu->regular, p->menu->focused, p->menu->hovered,
		p->menu->pressed, p->menu->toggled, p->font, "New Line");

	p->menu->delete_selected_btn = GUI_button(
		20, 160,
		250, 30, 5, GUI_LINE_BORDER,
		p->menu->regular, p->menu->focused, p->menu->hovered,
		p->menu->pressed, p->menu->toggled, p->font, "Delete selected");

	p->menu->dimension_btn = GUI_button(
		20, 120,
		130, 30, 5, GUI_LINE_BORDER,
		p->menu->regular, p->menu->focused, p->menu->hovered,
		p->menu->pressed, p->menu->toggled, p->font, "Dimension");

	GUI_add(p->menu->main, p->menu->start_sketch_btn);
	GUI_add(p->menu->main, p->menu->projection_tgl);
	GUI_add(p->menu->main, p->menu->show_edges_tgl);
	GUI_add(p->menu->main, p->menu->extrude_btn);


	GUI_add(p->menu->sketch, p->menu->finish_sketch_btn);
	GUI_add(p->menu->sketch, p->menu->line_btn);
	GUI_add(p->menu->sketch, p->menu->dimension_btn);
	GUI_add(p->menu->sketch, p->menu->delete_selected_btn);
	GUI_add(p->menu->sketch, p->menu->sketch_exit_btn);

	GUI_add(p->menu->extrude, p->menu->extrude_exit_btn);
	GUI_add(p->menu->extrude, p->menu->extrude_dist_text_field);
	GUI_add(p->menu->extrude, p->menu->extrude_dist_lbl);
	GUI_add(p->menu->extrude, p->menu->extrude_select_btn);
	GUI_add(p->menu->extrude, p->menu->extrude_finish_btn);

	p->menu->contexts[0] = p->menu->main;
	p->menu->contexts[1] = p->menu->sketch;
	p->menu->contexts[2] = p->menu->extrude;
	p->menu->ctx_count = 3;
}