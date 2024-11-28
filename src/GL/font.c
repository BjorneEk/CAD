/*==========================================================*
 *
 * @author Gustaf Franzén :: https://github.com/BjorneEk;
 *
 * font rendering
 *
 *==========================================================*/

#include "font.h"
#include "../matrix/matrix.h"
#include <stdlib.h>
#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string.h>
#include <stdarg.h>

font_t *new_font(const char *font_path)
{
	font_t *res;
	FT_Library ft;
	FT_Face face;
	u32_t texture;
	u8_t c;

	res = malloc(sizeof(font_t));

	shader_load(&res->shader, "src/shaders/text_vert.glsl", "src/shaders/text_frag.glsl");
	// All functions return a value different than 0 whenever an error occurred
	if (FT_Init_FreeType(&ft)) {
		fprintf(stderr,"ERROR::FREETYPE: Could not init FreeType Library\n");
		return NULL;
	}

	// load font as face
	if (FT_New_Face(ft, font_path, 0, &face)) {
		fprintf(stderr,"ERROR::FREETYPE: Failed to load font\n");
		return NULL;
	}

	FT_Set_Pixel_Sizes(face, 0, 48);

	// disable byte-alignment restriction
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	for (c = 0; c < 128; c++) {
		// Load character glyph
		if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
			fprintf(stderr,"ERROR::FREETYTPE: Failed to load Glyph\n");
			continue;
		}
		// generate texture
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);
		// set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// now store character for later use

		res->characters[c] = (FONT_CHAR){
			texture,
			V2i(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			V2i(face->glyph->bitmap_left, face->glyph->bitmap_top),
			(u32_t)(face->glyph->advance.x)
		};
	}
	glBindTexture(GL_TEXTURE_2D, 0);

	// destroy FreeType once we're finished
	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	glGenVertexArrays(1, &res->VAO);
	glGenBuffers(1, &res->VBO);
	glBindVertexArray(res->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, res->VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(f32_t) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(f32_t), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	return res;
}
void render_text_(font_t *font, f32_t x, f32_t y, f32_t scale, v3f_t color, f32_t *view, f32_t *proj, const char *text)
{
	u32_t i;
	f32_t x_pos;
	f32_t y_pos;
	f32_t w;
	f32_t h;
	FONT_CHAR ch;

	shader_use(&font->shader);
	shader_setV3(&font->shader, "text_color", color);
	shader_setV2(&font->shader, "pos", V2(0.0, 0.0));
	shader_setM4(&font->shader, "view", view);
	shader_setM4(&font->shader, "projection", proj);

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(font->VAO);

	// iterate through all characters
	for(i = 0; text[i] != '\0'; i++) {
		ch = font->characters[text[i]];
		//printf("texture: %c : %i\n", text[i], ch.texture_ID);
		x_pos = x + ch.bearing.x * scale;
		y_pos = y - (ch.size.y - ch.bearing.y) * scale;

		w = ch.size.x * scale;
		h = ch.size.y * scale;
		// update VBO for each character
		f32_t vertices[6][4] = {
			{ x_pos,	y_pos + h,	0.0f, 0.0f },
			{ x_pos,	y_pos,		0.0f, 1.0f },
			{ x_pos + w,	y_pos,		1.0f, 1.0f },

			{ x_pos,	y_pos + h,	0.0f, 0.0f },
			{ x_pos + w,	y_pos,		1.0f, 1.0f },
			{ x_pos + w,	y_pos + h,	1.0f, 0.0f }
		};
		// render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.texture_ID);
		// update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, font->VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (ch.advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
	}
	glEnable(GL_DEPTH_TEST);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

f32_t font_str_width(font_t *font, char *str, f32_t scale)
{
	f32_t len;
	i32_t i;
	len = 0;
	for(i = 0; str[i] != '\0'; i++)
		len += (font->characters[str[i]].advance >> 6) * scale;
	return len;
}
f32_t font_strn_width(font_t *font, char *str, u32_t n, f32_t scale)
{
	f32_t len;
	i32_t i;
	len = 0;
	for(i = 0; i < n; i++)
		len += (font->characters[str[i]].advance >> 6) * scale;
	return len;
}

f32_t font_str_height(font_t *font, char *str, f32_t scale)
{
	f32_t max;
	f32_t current;
	i32_t i;
	FONT_CHAR ch;
	max = 0;
	for(i = 0; str[i] != '\0'; i++) {
		ch = font->characters[str[i]];
		current = ch.bearing.y * scale;
		max = (current > max) ? current : max;
	}

	return max;
}
void render_text(font_t *font, f32_t x, f32_t y, f32_t scale, v3f_t color, f32_t *proj, const char *text)
{
	f32_t *model;
	f32_t *view;
	model = M4_eye();
	view = M4_eye();
	render_text_(font, x, y, scale, color, view, proj, text);
	free(model);
	free(view);

}

void render_textf_(font_t *font, f32_t x, f32_t y, f32_t scale, v3f_t color, f32_t *view, f32_t *proj, const char *fmt, ...)
{
	va_list args;
	char *buff;
	i32_t bytes;
	i32_t res_bytes;

	va_start(args, fmt);

	bytes = strlen(fmt);
	buff = malloc(bytes);
	res_bytes = vsnprintf(buff, bytes, fmt, args);

	while(res_bytes >= bytes - 1) {
		bytes *= 10;
		buff = realloc(buff, bytes);
		va_start(args, fmt);
		res_bytes = vsnprintf(buff, bytes, fmt, args);
	}
	//printf("BUFF: %s\n", buff);
	render_text_(font, x, y, scale, color, view, proj, buff);

	free(buff);
}

void render_textf(font_t *font, f32_t x, f32_t y, f32_t scale, v3f_t color, f32_t *proj, const char *fmt, ...)
{
	va_list args;
	char *buff;
	i32_t bytes;
	i32_t res_bytes;

	va_start(args, fmt);

	bytes = strlen(fmt);
	buff = malloc(bytes);
	res_bytes = vsnprintf(buff, bytes, fmt, args);

	while(res_bytes >= bytes - 1) {
		bytes *= 10;
		buff = realloc(buff, bytes);
		va_start(args, fmt);
		res_bytes = vsnprintf(buff, bytes, fmt, args);
	}
	//printf("BUFF: %s\n", buff);
	render_text(font, x, y, scale, color, proj, buff);

	free(buff);
}