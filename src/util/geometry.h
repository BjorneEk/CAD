/*==========================================================*
 *
 * @author Gustaf Franzén :: https://github.com/BjorneEk;
 *
 * functions for generating geometry
 *
 *==========================================================*/

#ifndef _GEOMETRY_H_
#define _GEOMETRY_H_

#include "../matrix/vector.h"

v2f_t circle_cord(f32_t rad, f32_t angle);

f32_t *gen_sector(v2f_t org, f32_t radius, f32_t start_angle, f32_t span_angle, u32_t triangle_count);

void append_rect(f32_t *verts, v2f_t org, f32_t width, f32_t height);

f32_t *rounded_rect(v2f_t org, f32_t width, f32_t height, f32_t radius, u32_t *floats);

bool rounded_rect_contains(v2f_t pos, f32_t width, f32_t height, f32_t radius, v2f_t p);
#endif /* _GEOMETRY_H_ */