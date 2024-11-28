/*==========================================================*
 *
 * @author Gustaf Franzén :: https://github.com/BjorneEk;
 *
 * gui components
 *
 *==========================================================*/

#include "GUI.h"
#include "GUI_component.h"
#include "../util/geometry.h"
#include "../util/glutils.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>

void	GUI_mouse_button_update(GUI_ctx *ctx, i32_t button, i32_t action, i32_t mods)
{
	if (!ctx->enabled || button != GLFW_MOUSE_BUTTON_LEFT)
		return;

	FOREACH_DLA(ctx->components, i, GUI_component*, c, {
		if (action && c->hovered) {
			GUI_press_component(c);
		} else if (c->hovered) {
			GUI_release_component(c);
		} else {
			c->pressed = 0;
			if (c->focused) {
				ctx->focused = NULL;
				GUI_unfocus_component(c);
			}
		}
	});
}
void	GUI_mouse_motion_update(GUI_ctx *ctx, f64_t x_in, f64_t y_in)
{
	if (!ctx->enabled)
		return;
	FOREACH_DLA(ctx->components, i, GUI_component*, c, {
		if (!c->hovered && GUI_component_contains(c, V2_sub(V2(x_in, y_in), ctx->pos))) {
			GUI_hover_component(c);
		} else if (c->hovered && !GUI_component_contains(c, V2_sub(V2(x_in, y_in), ctx->pos))) {
			GUI_unhover_component(c);
		}
	});
}
void	GUI_character_update(GUI_ctx *ctx, u32_t codepoint)
{
	if (!ctx->enabled)
		return;
	FOREACH_DLA(ctx->components, i, GUI_component*, c, {
		if (c->focused && c->type == GUI_TEXT_FIELD) {
			c->keyboard_event.codepoint = codepoint;
			GUI_input_character_component(c);
		}
	});
}

void	GUI_key_update(GUI_ctx *ctx, i32_t key, i32_t scancode, i32_t action, i32_t mods)
{
	if (!ctx->enabled)
		return;
	FOREACH_DLA(ctx->components, i, GUI_component*, c, {
		if (action == GLFW_PRESS && c->focused) {
			c->keyboard_event.key = key;
			c->keyboard_event.scancode = scancode;
			c->keyboard_event.action = action;
			c->keyboard_event.mods = mods;
			GUI_keypress_component(c);
		}
	});
}
void	GUI_scroll_callback(GUI_ctx *ctx, f64_t x_in, f64_t y_in);
void	GUI_framebuffer_size_callback(GUI_ctx ctx, i32_t width, i32_t height);
void	GUI_mouse_dragged(GUI_ctx *ctx, i32_t button, f64_t x_in, f64_t y_in);

GUI_ctx	*GUI_new_rounded(v2f_t pos, u32_t width, u32_t height, f32_t radius, f32_t border_width,
	v4f_t background_color, v4f_t border_color, u32_t scr_width, u32_t scr_height)
{
	GUI_ctx *res;
	f32_t *mesh;
	u32_t bg_size;

	res = malloc(sizeof(GUI_ctx));

	res->components = DLA_new(sizeof(GUI_component*), 5);
	res->proj = M4_orthographic(0.0, scr_height, 0.0, scr_width);
	res->focused = NULL;
	res->width = width;
	res->height = height;
	res->pos = pos;
	res->enabled = true;


	mesh = rounded_rect(V2(0.0, 0.0), width, height, radius, &bg_size);
	res->border = V2_VAO(mesh, bg_size);
	free(mesh);
	mesh = rounded_rect(
		V2(border_width, border_width),
		width - 2*border_width, height - 2*border_width, radius - border_width,
		&bg_size);
	res->background = V2_VAO(mesh, bg_size);
	free(mesh);

	res->background_color = background_color;
	res->border_color = border_color;

	res->has_border = true;
	res->radius = radius;
	res->pressed = false;
	res->hovered = false;
	return res;
}


GUI_ctx *GUI_new(v2f_t pos, u32_t width, u32_t height, v4f_t background_color, u32_t scr_width, u32_t scr_height)
{
	GUI_ctx *res;
	f32_t *mesh;
	u32_t bg_size;

	res = malloc(sizeof(GUI_ctx));

	res->components = DLA_new(sizeof(GUI_component*), 5);
	res->proj = M4_orthographic(0.0, scr_height, 0.0, scr_width);
	res->focused = NULL;
	res->width = width;
	res->height = height;
	res->pos = pos;
	res->enabled = true;


	mesh = rounded_rect(V2(0.0, 0.0), width, height, 0, &bg_size);
	res->background = V2_VAO(mesh, bg_size);
	free(mesh);

	res->background_color = background_color;

	res->has_border = false;
	res->radius = 0;
	res->movable = false;
	res->pressed = false;
	res->hovered = false;
	return res;
}


void gui_focus_listener(GUI_component *c)
{
	c->ctx->focused = c;
	GUI_focus_component(c);
}

void GUI_add(GUI_ctx *ctx, GUI_component *c)
{
	c->ctx = ctx;
	GUI_add_listener(c, GUI_CLICKED, gui_focus_listener);
	DLA_append(ctx->components, &c);
}

bool GUI_is_free(GUI_ctx *ctx, v2f_t pos)
{
	FOREACH_DLA(ctx->components, i, GUI_component*, c, {
		if (GUI_component_contains(c, V2_sub(pos, ctx->pos))) {
			return false;
		}
	});
	return true;
}

bool GUI_contains(GUI_ctx *ctx, v2f_t pos)
{
	//return true;
	return rounded_rect_contains(ctx->pos, ctx->width, ctx->height, ctx->radius, pos);
	// ctx->enabled && (pos.x >= ctx->pos.x && pos.x <= ctx->pos.x + ctx->width && pos.y >= ctx->pos.y && pos.y <= ctx->pos.y + ctx->height);
}

void GUI_draw(GUI_ctx *ctx)
{
	shader_t shader;

	if (!ctx->enabled)
		return;
	shader = button_shader();
	shader_use(&shader);
	if (ctx->has_border) {
		shader_setV4(&shader, "color", ctx->border_color);
		shader_setM4(&shader, "projection", ctx->proj);
		shader_setV2(&shader, "pos", ctx->pos);
		glClear(GL_DEPTH_BUFFER_BIT);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_BLEND);
		glDisable(GL_CULL_FACE);
		glBindVertexArray(ctx->border.VAO);
		glDrawArrays(GL_TRIANGLES, 0, ctx->border.length);
	}

	shader_setV4(&shader, "color", ctx->background_color);
	shader_setM4(&shader, "projection", ctx->proj);
	shader_setV2(&shader, "pos", ctx->pos);
	glClear(GL_DEPTH_BUFFER_BIT);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glEnable(GL_BLEND);
	//glDisable(GL_CULL_FACE);
	glBindVertexArray(ctx->background.VAO);
	glDrawArrays(GL_TRIANGLES, 0, ctx->background.length);


	FOREACH_DLA(ctx->components, i, GUI_component*, c, {
		if (GUI_contains(ctx, V2_add(ctx->pos, c->pos)))
			GUI_draw_component(c, ctx->pos, ctx->proj);
	});
	glBindVertexArray(0);
}

void GUI_destroy(GUI_ctx **ctx)
{
	DLA_free(&(*ctx)->components);
	free((*ctx)->proj);
	free(*ctx);
}