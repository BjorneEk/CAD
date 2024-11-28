/*==========================================================*
 *
 * @author Gustaf Franzén :: https://github.com/BjorneEk;
 *
 * complete cycle sketch representation
 *
 *==========================================================*/

#ifndef _SEGMENT_H_
#define _SEGMENT_H_

#include "sketch.h"

typedef struct cycle {

	v3f_t normal;
	v3f_t *verts;
	u64_t *cycle;
	line_t *lines;
	u64_t length;

	u32_t *mesh;
	u64_t mesh_len;
	u32_t EBO;
	u32_t VAO;
	u32_t VBO;

	bool	hovered;
	bool	selected;
} cycle_t;

typedef struct unclosed {
	line_t *lines;
	u32_t length;
} unclosed_t;

typedef struct segment {
	u32_t type;
	cycle_t cycle;
	line_t *lines;
	u32_t length;
	selectable_t parent;
} segment_t;

void reverse(u64_t *arr, u64_t len);


bool cycle_contains(cycle_t c, v3f_t p);

void show_segment(segment_t *seg, shader_t shader, font_t *font, f32_t *view, f32_t *proj, bool selected, bool hovered);

bool segment_try_select(segment_t *seg, v3f_t real_pos, segment_t **res);

/* selectable_t*/
dla_t *finish_sketch(sketch_t **sketch);

#endif /* _SEGMENT_H_ */