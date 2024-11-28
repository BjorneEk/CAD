/*==========================================================*
 *
 * @author Gustaf Franzén :: https://github.com/BjorneEk;
 *
 * listener functions for gui
 *==========================================================*/

#include "GUI/GUI_component.h"
#ifndef _LISTENERS_H_
#define _LISTENERS_H_


void toggle_projection_type_listener(GUI_component *c);

void hand_cursor_listener(GUI_component *c);

void cursor_onhover_listener(GUI_component *c);
void cursor_reset_listener(GUI_component *c);
void crosshair_cursor_listener(GUI_component *c);


/**
 * attempts to start a sketch
 */
void sketch_listener(GUI_component *c);

void draw_line_listener(GUI_component *c);
void exit_sketch_listener(GUI_component *c);

void delete_selected_listener(GUI_component *c);

void extrude_listener(GUI_component *c);
void select_extrude_geometry_listener(GUI_component *c);
void exit_extrude_listener(GUI_component *c);
void extrude_dist_updated_listener(GUI_component *c);
void finish_extrusion_listener(GUI_component *c);

void toggle_show_edges_listener(GUI_component *c);
#endif /* _LISTENERS_H_ */