/*==========================================================*
 *
 * @author Gustaf Franzén :: https://github.com/BjorneEk;
 *
 * font rendering
 *
 *==========================================================*/

#ifndef _FONT_H_
#define _FONT_H_

#include "shader.h"
#include "../util/types.h"
#include "../matrix/vector.h"
#include <ft2build.h>
#include FT_FREETYPE_H

typedef struct character {
	u32_t texture_ID;	// ID handle of the glyph texture
	v2i_t size;		// Size of glyph
	v2i_t bearing;		// Offset from baseline to left/top of glyph
	u32_t advance;		// Horizontal offset to advance to next glyph
} FONT_CHAR;

typedef struct font {
	FONT_CHAR characters[128];
	u32_t VAO;
	u32_t VBO;
	shader_t shader;
} font_t;


font_t *new_font(const char *font_path);

void render_text_(font_t *font, f32_t x, f32_t y, f32_t scale, v3f_t color, f32_t *view, f32_t *proj, const char *text);

void render_text(font_t *font, f32_t x, f32_t y, f32_t scale, v3f_t color, f32_t *proj, const char *text);

void render_textf(font_t *font, f32_t x, f32_t y, f32_t scale, v3f_t color, f32_t *proj, const char *fmt, ...);

void render_textf_(font_t *font, f32_t x, f32_t y, f32_t scale, v3f_t color, f32_t *view, f32_t *proj, const char *fmt, ...);

f32_t font_str_width(font_t *font, char *str, f32_t scale);
f32_t font_strn_width(font_t *font, char *str, u32_t n, f32_t scale);
f32_t font_str_height(font_t *font, char *str, f32_t scale);

#endif /* _FONT_H_ */