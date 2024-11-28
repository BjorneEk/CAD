/*==========================================================*
 *
 * @author Gustaf Franzén :: https://github.com/BjorneEk;
 *
 * simple geometry drawing
 *
 *==========================================================*/

#ifndef _DRAW_H_
#define _DRAW_H_

#include "../matrix/vector.h"

void draw_dot(v3f_t p, f32_t radius, v4f_t color, f32_t *view, f32_t *proj);

void draw_dot_normal(v3f_t p, v3f_t normal, f32_t radius, v4f_t color, f32_t *view, f32_t *proj);

#endif /* */