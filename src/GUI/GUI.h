/*==========================================================*
 *
 * @author Gustaf Franzén :: https://github.com/BjorneEk;
 *
 * gui components
 *
 *==========================================================*/

#ifndef _GUI_H_
#define _GUI_H_

#include "../GL/globject.h"
#include "GUI_component.h"



typedef struct gui {

	dla_t *components;

	f32_t *proj;

	GUI_component *focused;

	v4f_t background_color;
	v4f_t border_color;

	bool has_border;
	bool movable;

	v2f_t pos;
	f32_t width;
	f32_t height;
	f32_t radius;

	GL_object_t background;
	GL_object_t border;

	bool enabled;
	bool pressed;
	bool hovered;
	v2f_t press_point;
} GUI_ctx;



void	GUI_mouse_button_update(GUI_ctx *ctx, i32_t button, i32_t action, i32_t mods);
void	GUI_mouse_motion_update(GUI_ctx *ctx, f64_t x_in, f64_t y_in);
void	GUI_scroll_callback(GUI_ctx *ctx, f64_t x_in, f64_t y_in);
void	GUI_framebuffer_size_callback(GUI_ctx ctx, i32_t width, i32_t height);
void	GUI_mouse_dragged(GUI_ctx *ctx, i32_t button, f64_t x_in, f64_t y_in);
void	GUI_character_update(GUI_ctx *ctx, u32_t codepoint);
void	GUI_key_update(GUI_ctx *ctx, i32_t key, i32_t scancode, i32_t action, i32_t mods);
bool	GUI_is_free(GUI_ctx *ctx, v2f_t pos);

void	GUI_new_free_panel(u32_t width, u32_t height, f32_t radius, f32_t border_width,
	v4f_t background_color, v4f_t border_color, u32_t scr_width, u32_t scr_height);

GUI_ctx *GUI_new(v2f_t pos, u32_t width, u32_t height, v4f_t background_color, u32_t scr_width, u32_t scr_height);
GUI_ctx	*GUI_new_rounded(v2f_t pos, u32_t width, u32_t height, f32_t radius, f32_t border_width,
	v4f_t background_color, v4f_t border_color, u32_t scr_width, u32_t scr_height);
void GUI_add(GUI_ctx *ctx, GUI_component *c);

void GUI_draw(GUI_ctx *ctx);

void GUI_destroy(GUI_ctx **ctx);

bool GUI_contains(GUI_ctx *ctx, v2f_t pos);
#endif /* _GUI_H_ */
