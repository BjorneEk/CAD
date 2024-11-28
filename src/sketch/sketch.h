/*==========================================================*
 *
 * @author Gustaf Franzén :: https://github.com/BjorneEk;
 *
 * sketch representation
 *
 *==========================================================*/

#ifndef _SKETCH_H_
#define _SKETCH_H_

#include "line.h"
#include "../util/types.h"
#include "../GL/font.h"
#include "../util/error.h"
#include "../util/glutils.h"
#include "../matrix/matrix.h"
#include "../matrix/vector.h"
#include "../GL/shader.h"
#include "../structures/dynamic_array.h"
#include "../selectable/selectable.h"




/* indiciecs is a array of inicies to lines in the lines array of a sketch*/
typedef struct sketch {
	dla_t	*lines;		/* sketch_line_t */
	v3f_t p1;
	bool	has_p1;
	selectable_t parent;
} sketch_t;



sketch_t *new_sketch(selectable_t parent);

void sketch_add(sketch_t *sketch, v3f_t p);

v3f_t sketch_normal(sketch_t *sketch);

void add_line(sketch_t *sketch, line_t line);

void show_sketch(sketch_t *sketch, shader_t shader, font_t *font, f32_t *view, f32_t *proj, v3f_t p2);

//void show_sketch_cycles(sketch_t *sketch, shader_t shader, font_t *font, f32_t *view, f32_t *proj);

void draw_line(line_t line, shader_t shader, font_t *font, v3f_t color, f32_t *view, f32_t *proj);

void try_snap_sketch(sketch_t *sketch, v3f_t *pos, f32_t snap_dist);

void delete_selected(sketch_t *sketch);
void sketch_select(sketch_t *sketch, v3f_t pos, f32_t select_dist);
void deselect_all(sketch_t *sketch);



#endif /* _SKETCH_H_ */