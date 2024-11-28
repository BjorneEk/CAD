/*==========================================================*
 *
 * @author Gustaf Franzén :: https://github.com/BjorneEk;
 *
 * gui components
 *
 *==========================================================*/

#ifndef _GUI_COMPONENT_H_
#define _GUI_COMPONENT_H_


#include "../util/types.h"
#include "../util/error.h"
#include "../matrix/matrix.h"
#include "../matrix/vector.h"
#include "../structures/dynamic_array.h"
#include "../GL/font.h"
#include "../GL/globject.h"
#include <stdbool.h>
#include <pthread.h>

typedef enum component_type {
	GUI_BUTTON,
	GUI_TOGGLE_BUTTON,
	GUI_LABEL,
	GUI_TEXT_FIELD
} GUI_type;

typedef enum border_style {
	GUI_LINE_BORDER,
	GUI_NO_BORDER
} GUI_border_style;

typedef struct text {
	f32_t	x;
	f32_t	y;
	f32_t	scale;
	v3f_t	color;
	u32_t	length;
	char	*str;
	font_t *font;
} GUI_text;

typedef struct GC_palette {
	v4f_t bg;
	v4f_t border;
} GUI_palette;

typedef enum listener_type {
	GUI_CLICKED,
	GUI_TOGGLED,
	GUI_HOVERED,
	GUI_UNHOVERED,
	GUI_FOCUS,
	GUI_UNFOCUS,
	GUI_KEY_PRESSED,
	GUI_TEXT_INPUT,
	GUI_TEXT_UPDATE,
} GUI_listener_type;

typedef struct gui GUI_ctx;

typedef struct animator {
	pthread_t thread;
	f32_t delay;
	f32_t rate;
	_Atomic bool animating;
} GUI_animator;

typedef struct component {
	GUI_type type;

	v2f_t pos;

	f32_t width;
	f32_t height;
	f32_t radius;

	shader_t shader;
	GL_object_t background;
	GL_object_t handle;
	GL_object_t cursor;

	v2f_t handle_positions[2];
	f32_t handle_pos_lerp;

	v4f_t handle_colors[2];
	f32_t handle_color_lerp;

	GUI_animator animator;

	u32_t cursor_pos;
	bool cursor_vissible;

	GUI_border_style border_style;
	GL_object_t border;
	shader_t border_shader;

	struct {
		GUI_palette *regular;
		GUI_palette *focused;
		GUI_palette *hovered;
		GUI_palette *pressed;
		GUI_palette *toggled;
	} palette;

	GUI_text text;
	GUI_text text2;

	struct {
		u8_t focused : 1;
		u8_t hovered : 1;
		u8_t pressed : 1;
		u8_t toggled : 1;
	};

	struct {
		u32_t codepoint;
		i32_t key;
		i32_t scancode;
		i32_t action;
		i32_t mods;
	} keyboard_event;

	dla_t *listeners;

	GUI_ctx *ctx;

	bool enabled;
	bool vissible;
} GUI_component;

typedef void (*GUI_listener_callback)(GUI_component*);

typedef struct listener {

	GUI_listener_type type;

	GUI_listener_callback callback;

} GUI_listener;

GUI_component	*GUI_new_component(
	GUI_type type,			v2f_t pos,
	f32_t width,			f32_t height, 		f32_t radius,
	shader_t shader, 		GL_object_t background,	GL_object_t handle,
	GUI_border_style border_style,	GL_object_t border,	shader_t border_shader,
	GUI_palette *regular,		GUI_palette *focused,	GUI_palette *hovered,
	GUI_palette *pressed,		GUI_palette *toggled,	GUI_text text);

GUI_component	*GUI_button(
	f32_t x,				f32_t y,
	f32_t width,				f32_t height,	f32_t radius,
	GUI_border_style border_style,		GUI_palette *regular,
	GUI_palette *focused,			GUI_palette *hovered,
	GUI_palette *pressed,			GUI_palette *toggled,
	font_t *font, 				char *label);

GUI_component	*GUI_label(
	f32_t x,				f32_t y,
	f32_t width,				f32_t height,	f32_t radius,
	GUI_border_style border_style,		GUI_palette *regular,
	font_t *font, 				char *label);

GUI_component	*GUI_toggle_button(
	f32_t x,				f32_t y,
	f32_t width,				f32_t height,	f32_t radius,
	GUI_border_style border_style,		GUI_palette *regular,
	GUI_palette *focused,			GUI_palette *hovered,
	GUI_palette *pressed,			GUI_palette *toggled,
	v4f_t untoggled_color,			v4f_t toggled_color,
	font_t *font, 				char *label1, char *label2);

GUI_component	*GUI_text_field(
	f32_t x,				f32_t y,
	f32_t width,				f32_t height,	f32_t radius,
	GUI_border_style border_style,		GUI_palette *regular,
	GUI_palette *focused,			GUI_palette *hovered,
	GUI_palette *pressed,			GUI_palette *toggled,
	font_t *font, 				char *label);

GUI_palette	*GUI_new_palette(v4f_t bg, v4f_t border);

GUI_palette	*GUI_blend_palettes(GUI_palette *a, GUI_palette *b);

void		GUI_add_listener(
		GUI_component	*c,
		GUI_listener_type type,
		GUI_listener_callback callback);

void		GUI_draw_component(GUI_component *c, v2f_t org, f32_t *proj);

bool		GUI_component_contains(GUI_component *c, v2f_t pos);

void		GUI_hover_component(GUI_component *c);
void		GUI_unhover_component(GUI_component *c);
void		GUI_press_component(GUI_component *c);
void		GUI_release_component(GUI_component *c);
void		GUI_unfocus_component(GUI_component *c);
void		GUI_focus_component(GUI_component *c);
void		GUI_input_character_component(GUI_component *c);
void		GUI_keypress_component(GUI_component *c);

void		GUI_destroy_component(GUI_component **c);

void		GUI_toggle(GUI_component *c);

void append_rect(f32_t *verts, v2f_t org, f32_t width, f32_t height);

shader_t button_shader();
#endif /* _GUI_COMPONENT_H_ */