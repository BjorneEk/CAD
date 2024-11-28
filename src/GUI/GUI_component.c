

#include "GUI_component.h"
#include "../util/glutils.h"
#include "../util/geometry.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <string.h>


static const char *BUTTON_VERT_PATH = "src/shaders/GUI/default_vert.glsl";
static const char *BUTTON_FRAG_PATH = "src/shaders/GUI/default_frag.glsl";

extern f32_t GLOBL_DELTA_TIME;

shader_t button_shader()
{
	static shader_t shader;
	static bool shader_loaded = false;

	if (shader_loaded)
		return shader;
	else
		shader_load(&shader, BUTTON_VERT_PATH, BUTTON_FRAG_PATH);
	shader_loaded = true;
	return shader;
}

GUI_component	*GUI_new_component(
	GUI_type type,			v2f_t pos,
	f32_t width,			f32_t height, 		f32_t radius,
	shader_t shader, 		GL_object_t background,	GL_object_t handle,
	GUI_border_style border_style,	GL_object_t border,	shader_t border_shader,
	GUI_palette *regular,		GUI_palette *focused,	GUI_palette *hovered,
	GUI_palette *pressed,		GUI_palette *toggled,	GUI_text text)
{
	GUI_component	*res;

	res = malloc(sizeof(GUI_component));

	res->type		= type;
	res->pos		= pos;
	res->width		= width;
	res->height		= height;
	res->radius		= radius;
	res->shader		= shader;
	res->background		= background;
	res->handle		= handle;
	res->border_style	= border_style;
	res->border		= border;
	res->border_shader	= border_shader;
	res->palette.regular	= regular;
	res->palette.focused	= focused;
	res->palette.hovered	= hovered;
	res->palette.pressed	= pressed;
	res->palette.toggled	= toggled;
	res->text		= text;
	res->focused		= 0;
	res->hovered		= 0;
	res->pressed		= 0;
	res->toggled		= 0;
	res->ctx		= NULL;
	res->enabled		= true;
	res->cursor_pos		= 0;
	res->animator = (GUI_animator){0, 0, false};

	res->listeners = DLA_new(sizeof (GUI_listener), 2);

	return res;
}
/*
static inline bool inside_rect(v2f_t org, f32_t width, f32_t height, v2f_t pos)
{
	return pos.x >= org.x && pos.x <= org.x + width && pos.y >= org.y && pos.y <= org.y + height;
}
static inline bool inside_circle(v2f_t org, f32_t radius, v2f_t pos)
{
	return V2_len(V2_sub(org, pos)) <= radius;
}
*/
bool		GUI_component_contains(GUI_component *c, v2f_t pos)
{
	return rounded_rect_contains(c->pos, c->width, c->height, c->radius, pos);
}

void		GUI_hover_component(GUI_component *c)
{
	c->hovered = 1;
	FOREACH_DLA(c->listeners, i, GUI_listener, l, {
		if (l.type == GUI_HOVERED)
			l.callback(c);
	});
}
void		GUI_unhover_component(GUI_component *c)
{
	c->hovered = 0;
	FOREACH_DLA(c->listeners, i, GUI_listener, l, {
		if (l.type == GUI_UNHOVERED)
			l.callback(c);
	});
}
void		GUI_press_component(GUI_component *c)
{
	if (!c->enabled)
		return;
	c->pressed = 1;
}
void		GUI_release_component(GUI_component *c)
{
	if (!c->pressed)
		return;
	if (!c->enabled)
		return;

	c->pressed = 0;
	FOREACH_DLA(c->listeners, i, GUI_listener, l, {
		if (l.type == GUI_CLICKED)
			l.callback(c);
	});
}

void		GUI_focus_component(GUI_component *c)
{
	if (!c->enabled)
		return;
	c->focused = 1;
	FOREACH_DLA(c->listeners, i, GUI_listener, l, {
		if (l.type == GUI_FOCUS)
			l.callback(c);
	});
}

void		GUI_unfocus_component(GUI_component *c)
{
	if (!c->enabled)
		return;
	c->focused = 0;
	FOREACH_DLA(c->listeners, i, GUI_listener, l, {
		if (l.type == GUI_UNFOCUS)
			l.callback(c);
	});
}

void GUI_input_character_component(GUI_component *c)
{
	if (!c->enabled)
		return;

	FOREACH_DLA(c->listeners, i, GUI_listener, l, {
		if (l.type == GUI_TEXT_UPDATE)
			l.callback(c);
	});
}
void GUI_keypress_component(GUI_component *c)
{
	if (!c->enabled)
		return;

	FOREACH_DLA(c->listeners, i, GUI_listener, l, {
		if (l.type == GUI_KEY_PRESSED)
			l.callback(c);
	});
}

void GUI_text_inputed_component(GUI_component *c)
{
	if (!c->enabled)
		return;

	FOREACH_DLA(c->listeners, i, GUI_listener, l, {
		if (l.type == GUI_TEXT_INPUT)
			l.callback(c);
	});
}

GUI_component	*GUI_button(
	f32_t x,				f32_t y,
	f32_t width,				f32_t height,	f32_t radius,
	GUI_border_style border_style,		GUI_palette *regular,
	GUI_palette *focused,			GUI_palette *hovered,
	GUI_palette *pressed,			GUI_palette *toggled,
	font_t *font, 				char *label)
{
	GL_object_t background;
	GL_object_t border;
	u32_t border_width;
	GUI_text text;
	f32_t *mesh;
	u32_t bg_size;

	f32_t label_length;
	f32_t max_lbl_height;

	border_width = 1;

	max_lbl_height = font_str_height(font, label, 1.0);
	text.scale = (height - 12*border_width) / max_lbl_height;
	label_length = font_str_width(font, label, text.scale);
	text.x = x + (width / 2) - (label_length / 2);
	text.y = y + 6*border_width;

	text.color = V3(0.0, 0.0, 0.0);
	text.length = strlen(label);
	text.str = label;
	text.font = font;
	printf("length: %f, x: %f\n", label_length, text.x);

	mesh = rounded_rect(V2(0.0, 0.0), width, height, radius, &bg_size);
	background = V2_VAO(mesh, bg_size);
	free(mesh);

	if (border_style == GUI_LINE_BORDER) {
		border = background;
		mesh = rounded_rect(
			V2(border_width, border_width),
			width - 2*border_width, height - 2*border_width, radius - border_width,
			&bg_size);
		background = V2_VAO(mesh, bg_size);
		free(mesh);
	}

	return GUI_new_component(
		GUI_BUTTON,		V2(x, y),
		width,			height, 	radius,
		button_shader(),	background,
		(GL_object_t){0},		border_style,
		border,	button_shader(),
		regular,		focused,	hovered,
		pressed,		toggled,	text);
}

GUI_component	*GUI_label(
	f32_t x,				f32_t y,
	f32_t width,				f32_t height,	f32_t radius,
	GUI_border_style border_style,		GUI_palette *regular,
	font_t *font, 				char *label)
{
	GUI_component	*res;

	res = GUI_button(x, y, width, height, radius, border_style, regular, regular, regular, regular, regular, font, label);
	res->type = GUI_LABEL;
	return res;
}
void *animate_toggle_slider(void *in)
{
	GUI_component *c = in;
	f32_t timer = 0;
	static u32_t steps = 0;
	f32_t animation_rate = 0.01;

	timer = glfwGetTime();

	while (1) {
		if (glfwGetTime() >= timer) {
			timer += c->animator.delay;
			if (c->handle_pos_lerp < c->toggled) {
				c->handle_pos_lerp	+= c->animator.rate;
				c->handle_color_lerp	+= c->animator.rate;
			} else {
				c->handle_pos_lerp	-= c->animator.rate;
				c->handle_color_lerp	-= c->animator.rate;
			}
			if (c->toggled && c->handle_pos_lerp >= 1.0) {
				c->handle_pos_lerp	= 1.0;
				c->handle_color_lerp	= 1.0;
				break;
			} else if (!c->toggled && c->handle_pos_lerp <= 0.0) {
				c->handle_pos_lerp	= 0.0;
				c->handle_color_lerp	= 0.0;
				break;
			}
		}
	}
	c->animator.animating = false;
	return NULL;
}
void *animate_cursor_blink(void *in)
{
	GUI_component *c = in;
	f32_t timer = 0;
	static u32_t steps = 0;
	f32_t animation_rate = 0.01;

	timer = glfwGetTime();

	while (c->focused) { /* race condition city, replace with mutex */
		if (glfwGetTime() >= timer) {
			timer += c->animator.delay;
			c->cursor_vissible = !c->cursor_vissible;
		}
	}
	c->cursor_vissible = false;
	c->animator.animating = false;
	return NULL;
}
void spawn_toggle_slider_animator(GUI_component *c)
{
	c->animator.animating = true;

	pthread_create(
			&c->animator.thread,
			NULL,
			&animate_toggle_slider,
			c);
}
void spawn_cursor_blink_animator(GUI_component *c)
{
	if (c->animator.animating)
		return;

	c->animator.animating = true;

	pthread_create(
			&c->animator.thread,
			NULL,
			&animate_cursor_blink,
			c);

}

void toggle_listener(GUI_component *c)
{
	c->toggled = !c->toggled;

	if (!c->animator.animating)
		spawn_toggle_slider_animator(c);

	FOREACH_DLA(c->listeners, i, GUI_listener, l, {
		if (l.type == GUI_TOGGLED)
			l.callback(c);
	});
}

void text_input_listener(GUI_component *c)
{
	char *old;
	char *new;


	if ((i32_t)c->keyboard_event.codepoint > 128)
		printf("unsuported: %c, %i\n", c->keyboard_event.codepoint, c->keyboard_event.codepoint);
	old = c->text.str;
	new = malloc(((c->text.length + 1) + 1) * sizeof(char));
	memcpy(new, old, c->cursor_pos * sizeof(char));
	new[c->cursor_pos] = c->keyboard_event.codepoint;
	memcpy(new + c->cursor_pos + 1, old + c->cursor_pos, ((c->text.length + 1) - c->cursor_pos) * sizeof(char));
	free(old);
	c->text.str = new;
	++c->text.length;
	++c->cursor_pos;
	c->cursor_vissible = true;
}
void text_field_key_listener(GUI_component *c)
{
	c->cursor_vissible = true;
	switch(c->keyboard_event.key) {
		case GLFW_KEY_BACKSPACE:
			if (c->cursor_pos <= 0 || c->text.length <= 0)
				return;
			if (c->text.length == 1) {
				c->text.str[0] = '\0';
				c->text.length = 0;
				c->cursor_pos = 0;
				return;
			}
			--c->cursor_pos;
			strcpy(c->text.str + c->cursor_pos, c->text.str + c->cursor_pos + 1);
			--c->text.length;
			break;
		case GLFW_KEY_ENTER:
			GUI_text_inputed_component(c);
			c->focused = false;
			break;
		case GLFW_KEY_LEFT:
			if (c->cursor_pos > 0)
				--c->cursor_pos;
			break;
		case GLFW_KEY_RIGHT:
			if (c->cursor_pos < c->text.length)
				++c->cursor_pos;
			break;
	}
}
void GUI_toggle(GUI_component *c)
{
	toggle_listener(c);
}

GUI_component	*GUI_toggle_button(
	f32_t x,				f32_t y,
	f32_t width,				f32_t height,	f32_t radius,
	GUI_border_style border_style,		GUI_palette *regular,
	GUI_palette *focused,			GUI_palette *hovered,
	GUI_palette *pressed,			GUI_palette *toggled,
	v4f_t untoggled_color,			v4f_t toggled_color,
	font_t *font, 				char *label1, char *label2)
{
	GUI_component	*res;
	GL_object_t background;
	GL_object_t border;
	GL_object_t handle;
	u32_t border_width = 1;
	GUI_text text1, text2;
	f32_t *mesh;
	u32_t background_size;
	u32_t border_size;
	u32_t handle_size;
	v2f_t handle_pos[2];

	f32_t handle_width;
	f32_t handle_height;
	f32_t handle_rad;

	f32_t label_width;
	f32_t max_lbl_height;

	max_lbl_height = font_str_height(font, label1, 1.0);
	text1.scale = (height - 12*border_width) / max_lbl_height;
	label_width = font_str_width(font, label1, text1.scale);
	text1.x = x - label_width - 6*border_width;
	text1.y = y + 6*border_width;

	//text.x		= x + radius;
	//text.y		= height + y + height * 0.1;
	//text.scale	= 0.01 * height;
	text1.color	= V3(0.0, 0.0, 0.0);
	text1.length	= strlen(label1);
	text1.str	= label1;
	text1.font = font;

	max_lbl_height = font_str_height(font, label2, 1.0);
	text2.scale = (height - 12*border_width) / max_lbl_height;
	label_width = font_str_width(font, label2, text2.scale);
	text2.x = x + 6*border_width + width;
	text2.y = y + 6*border_width;

	//text.x		= x + radius;
	//text.y		= height + y + height * 0.1;
	//text.scale	= 0.01 * height;
	text2.color	= V3(0.0, 0.0, 0.0);
	text2.length	= strlen(label2);
	text2.str	= label2;
	text2.font = font;



	mesh = rounded_rect(V2(0.0, 0.0), width, height, radius, &background_size);
	background = V2_VAO(mesh, background_size);
	free(mesh);

	handle_pos[0] = handle_pos[1] = V2(0.0, 0.0);
	handle_width = width / 2;
	handle_height = height;
	handle_rad = radius;
	if (border_style == GUI_LINE_BORDER) {
		border = background;
		mesh = rounded_rect(
			V2(border_width, border_width),
			width - 2*border_width, height - 2*border_width, radius - border_width,
			&border_size);
		background = V2_VAO(mesh, border_size);
		free(mesh);
		handle_width = (width - 4*border_width) / 2;
		handle_height = (height - 4*border_width);
		handle_pos[0].x = 2*border_width;
		handle_pos[0].y = 2*border_width;
		handle_rad -= 2*border_width;
	}
	handle_pos[0].x += 2*border_width;
	handle_pos[0].y += 2*border_width;
	handle_pos[1].x = handle_width + handle_pos[0].x;
	handle_pos[1].y = handle_pos[0].y;

	mesh = rounded_rect(
		V2(0, 0),
		handle_width - 4*border_width, handle_height - 4*border_width, handle_rad - 2*border_width,
		&handle_size);
	handle = V2_VAO(mesh, handle_size);
	free(mesh);

	res = GUI_new_component(
		GUI_TOGGLE_BUTTON,		V2(x, y),
		width,			height, 	radius,
		button_shader(),	background,
		handle,	border_style,
		border,	button_shader(),
		regular,		focused,	hovered,
		pressed,		toggled,	text1);
	res->handle_positions[0] = handle_pos[0];
	res->handle_positions[1] = handle_pos[1];
	res->handle_pos_lerp = 0.0;
	res->handle_colors[0] = untoggled_color;
	res->handle_colors[1] = toggled_color;
	res->handle_color_lerp = 0.0;
	res->animator.delay = 0.001;
	res->animator.rate = 0.01;
	res->text2 = text2;
	GUI_add_listener(res, GUI_CLICKED, toggle_listener);
	return res;
}

GUI_component	*GUI_text_field(
	f32_t x,				f32_t y,
	f32_t width,				f32_t height,	f32_t radius,
	GUI_border_style border_style,		GUI_palette *regular,
	GUI_palette *focused,			GUI_palette *hovered,
	GUI_palette *pressed,			GUI_palette *toggled,
	font_t *font, 				char *lbl)
{
	GUI_component	*res;
	GL_object_t background;
	GL_object_t border;
	u32_t border_width;
	GUI_text text;
	f32_t *mesh;
	u32_t background_size;
	u32_t border_size;

	f32_t label_length;
	f32_t max_lbl_height;

	border_width = 1;

	max_lbl_height = font_str_height(font, lbl, 1.0);
	text.scale = (height - 12*border_width) / max_lbl_height;
	label_length = font_str_width(font, lbl, text.scale);

	text.x = x + 6*border_width;
	text.y = y + 6*border_width;

	text.color = V3(0.0, 0.0, 0.0);
	text.length = strlen(lbl);
	text.str = malloc((text.length+1) * sizeof(char));
	strcpy(text.str, lbl);
	text.font = font;
	printf("length: %f, x: %f\n", label_length, text.x);

	mesh = rounded_rect(V2(0.0, 0.0), width, height, radius, &background_size);
	background = V2_VAO(mesh, background_size);
	free(mesh);



	if (border_style == GUI_LINE_BORDER) {
		border = background;
		mesh = rounded_rect(
			V2(border_width, border_width),
			width - 2*border_width, height - 2*border_width, radius - border_width,
			&background_size);
		background = V2_VAO(mesh, background_size);
		free(mesh);
	}


	res = GUI_new_component(
		GUI_TEXT_FIELD,		V2(x, y),
		width,			height, 	radius,
		button_shader(),	background,
		(GL_object_t){0},		border_style,
		border,	button_shader(),
		regular,		focused,	hovered,
		pressed,		toggled,	text);
	f32_t cursor_mesh[4] = {0.0f, 0.0f, 0.0f, height - 4};

	res->cursor = V2_VAO(cursor_mesh, 4);

	res->cursor_pos = text.length;
	res->cursor_vissible = false;
	res->animator.animating = false;
	res->animator.delay = 0.4;
	GUI_add_listener(res, GUI_FOCUS, spawn_cursor_blink_animator);
	GUI_add_listener(res, GUI_TEXT_UPDATE, text_input_listener);
	GUI_add_listener(res, GUI_KEY_PRESSED, text_field_key_listener);
	return res;
}
GUI_palette	*GUI_new_palette(v4f_t bg, v4f_t border)
{
	GUI_palette *res;

	res = malloc(sizeof(GUI_palette));
	res->bg		= bg;
	res->border	= border;

	return res;
}

void		GUI_add_listener(
		GUI_component	*c,
		GUI_listener_type type,
		GUI_listener_callback callback)
{
	GUI_listener listener;

	listener = (GUI_listener) {
		type, callback
	};

	DLA_append(c->listeners, &listener);

	return;
}

GUI_palette *GUI_blend_palettes(GUI_palette *a, GUI_palette *b)
{
	return GUI_new_palette(V4_blend(a->bg, b->bg), V4_blend(a->border, b->border));
}

GUI_palette *get_palette(GUI_component *c)
{
	if (c->pressed)
		return c->palette.pressed;
	if (c->hovered)
		return c->palette.hovered;
	if (c->focused)
		return c->palette.focused;
	return c->palette.regular;
}
void draw_cursor(v2f_t p1, v2f_t p2)
{
	u32_t VAO;
	u32_t VBO;
	v2f_t line[2] = {p1, p2};
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(f32_t), &line, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glClear(GL_DEPTH_BUFFER_BIT);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glBindVertexArray(VAO);
	glDrawArrays(GL_LINES, 0, 4);
	//glBindVertexArray(0);
	//glDeleteVertexArrays(1, &VAO);
	//glDeleteBuffers(1, &VBO);

	//glClear(GL_DEPTH_BUFFER_BIT);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glEnable(GL_BLEND);
	//glEnable(GL_CULL_FACE);
	//glDisable(GL_BLEND);
}
void		GUI_draw_component(GUI_component *c, v2f_t org, f32_t *proj)
{
	GUI_palette *palette;
	v2f_t cursor_pos;
	palette = get_palette(c);

	if (c->border_style == GUI_LINE_BORDER) {
		shader_use(&c->border_shader);
		shader_setV4(&c->border_shader, "color", palette->border);
		shader_setM4(&c->border_shader, "projection", proj);
		shader_setV2(&c->border_shader, "pos", V2_add(org, c->pos));
		glClear(GL_DEPTH_BUFFER_BIT);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_BLEND);
		glDisable(GL_CULL_FACE);
		glBindVertexArray(c->border.VAO);
		glDrawArrays(GL_TRIANGLES, 0, c->border.length);
	}
	shader_use(&c->shader);
	shader_setV4(&c->shader, "color", palette->bg);
	shader_setM4(&c->shader, "projection", proj);
	shader_setV2(&c->shader, "pos", V2_add(org, c->pos));
	glClear(GL_DEPTH_BUFFER_BIT);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glBindVertexArray(c->background.VAO);
	glDrawArrays(GL_TRIANGLES, 0, c->background.length);
	if (c->type == GUI_TOGGLE_BUTTON) {
		fflush(stdout);
		shader_setV4(&c->shader, "color", V4_lerp(c->handle_color_lerp, c->handle_colors[0], c->handle_colors[1]));
		shader_setM4(&c->shader, "projection", proj);
		shader_setV2(&c->shader, "pos", V2_add(org, V2_add(c->pos, V2_lerp(c->handle_pos_lerp, c->handle_positions[0], c->handle_positions[1]))));
		glClear(GL_DEPTH_BUFFER_BIT);
		glBindVertexArray(c->handle.VAO);
		glDrawArrays(GL_TRIANGLES, 0, c->handle.length);
		glEnable(GL_CULL_FACE);
		glDisable(GL_BLEND);
		render_textf(c->text2.font, org.x + c->text2.x, org.y + c->text2.y, c->text2.scale, c->text2.color, proj, "%s", c->text2.str);
	} if (c->type == GUI_TEXT_FIELD && c->cursor_vissible) {
		cursor_pos = V2_add(org, V2(c->text.x + font_strn_width(c->text.font, c->text.str, c->cursor_pos, c->text.scale), c->pos.y + 2));
		shader_use(&c->shader);
		shader_setV4(&c->shader, "color", V4(0.0, 0.0, 0.0, 1.0));
		shader_setM4(&c->shader, "projection", proj);
		shader_setV2(&c->shader, "pos", cursor_pos);
		glClear(GL_DEPTH_BUFFER_BIT);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_BLEND);
		glDisable(GL_CULL_FACE);
		glBindVertexArray(c->cursor.VAO);
		glDrawArrays(GL_LINES, 0, c->cursor.length);

		//draw_cursor(V2(0.0, 0.0), V2(0.0, c->height - 4));
		//printf("x: %f, y: %f, x0: %f, y0, %f, x1: %f, y1, %f\n", cursor_pos.x, cursor_pos.y, org.x, org.y, V2(org.x, org.y + c->height - 4).x, V2(org.x, org.y + c->height - 4).y);
	}
	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);
	render_textf(c->text.font, org.x + c->text.x, org.y + c->text.y, c->text.scale, c->text.color, proj, "%s", c->text.str);
}

void		GUI_destroy_component(GUI_component **c)
{
	DLA_free(&(*c)->listeners);
	free(*c);
}