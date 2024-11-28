/*==========================================================*
 *
 * @author Gustaf Franzén :: https://github.com/BjorneEk;
 *
 * all listeners for gui components, these need acces to the
 * global variable "program", that holds the program context.
 *==========================================================*/

#include "program_state.h"
#include <stdio.h>
#include <string.h>

extern program_state_t *program;


void toggle_projection_type_listener(GUI_component *c)
{
	if (program->projection_type == PERSPECTIVE_PROJECTION)
		program->projection_type = ORTHOGRAPHIC_PROJECTION;
	else
		program->projection_type = PERSPECTIVE_PROJECTION;
	reset_view_soft(program);
}

void hand_cursor_listener(GUI_component *c)
{
	glfwSetCursor(program->window, program->cursor_hand);
}

void cursor_onhover_listener(GUI_component *c)
{
	if (c->type == GUI_BUTTON || c->type == GUI_TOGGLE_BUTTON)
		glfwSetCursor(program->window, program->cursor_hand);
	else if (c->type == GUI_TEXT_FIELD)
		glfwSetCursor(program->window, program->cursor_ibeam);
}
void cursor_reset_listener(GUI_component *c)
{
	glfwSetCursor(program->window, program->cursor_current);
}
void crosshair_cursor_listener(GUI_component *c)
{
	glfwSetCursor(program->window, program->cursor_crosshair);
}


/**
 * attempts to start a sketch
 */
void sketch_listener(GUI_component *c)
{
	if (program->mode != MODE_NORMAL)
		return;

	program->mode = MODE_ENTERING_SKETCH;
	if (program->selected != NULL)
		enter_sketch(program, *program->selected);
}

void draw_line_listener(GUI_component *c)
{
	program->mode = MODE_DRAWING_LINE;
	program->show_hover_point = true;
	deselect_all(program->sketch);
}

void exit_sketch_listener(GUI_component *c)
{
	program->show_hover_point = false;
	exit_sketch(program);
}

void delete_selected_listener(GUI_component *c)
{
	delete_selected(program->sketch);
}

void extrude_listener(GUI_component *c)
{
	if (program->mode != MODE_NORMAL)
		return;

	program->menu->extrude->pos = program->menu->start_pos;
	program->menu->extrude->enabled = true;

	program->extruder = new_extruder();
	//printf("extruder ptr: %p, to: %p\n", program->extruder, (void*)(((u64_t)program->extruder) + sizeof(extruder_t)));
	//printf("program ptr: %p, to: %p\n", program, (void*)(((u64_t)program) + sizeof(extruder_t)));
	program->mode = MODE_IN_EXTRUDE;

}
void select_extrude_geometry_listener(GUI_component *c)
{
	if (program->selected != NULL && program->selected->type == CYCLE) {
		set_extrude_geometry(program->extruder, program->selected);
		set_extruder_extrude_dist(program, 0.5);
	} else
		program->flags.selecting_geometry = 1;
}

void exit_extrude_listener(GUI_component *c)
{
	program->menu->extrude->enabled = false;
	program->mode = MODE_NORMAL;
	program->flags.selecting_geometry = 0;
	free_extruder(&program->extruder, false);
	strcpy(program->menu->extrude_dist_text_field->text.str, "0.0");
}
void extrude_dist_updated_listener(GUI_component *c)
{
	printf("updated\n");
}
void finish_extrusion_listener(GUI_component *c)
{
	selectable_t s;

	s = selectable(OBJECT, finish_extrusion(program->extruder), program->extruder->parent);
	free_extruder(&program->extruder, true);
	add_selectable(program, s);
	program->menu->extrude->enabled = false;
	program->mode = MODE_NORMAL;
	program->flags.selecting_geometry = 0;
	strcpy(program->menu->extrude_dist_text_field->text.str, "0.0");
}

void toggle_show_edges_listener(GUI_component *c)
{
	program->show_edges = !c->toggled;
}