/*==========================================================*
 *
 * @author Gustaf Franzén :: https://github.com/BjorneEk;
 *
 * vector functions
 *
 *==========================================================*/

#ifndef _VECTOR_H_
#define _VECTOR_H_

#include "../util/types.h"

typedef struct vector2f {
	f32_t x, y;
} v2f_t;

typedef struct vector3f {
	f32_t x, y, z;
} v3f_t;

typedef struct vector4f {
	f32_t x, y, z, w;
} v4f_t;

typedef struct vector2i {
	i32_t x, y;
} v2i_t;

typedef struct vector3i {
	i32_t x, y, z;
} v3i_t;

typedef struct vector4i {
	i32_t x, y, z, w;
} v4i_t;

v3f_t V3_add(v3f_t a, v3f_t b);

v3f_t V3_sub(v3f_t a, v3f_t b);

v3f_t V3_scale(v3f_t a, f32_t s);

f32_t V3_dot(v3f_t u, v3f_t v);

v3f_t V3_cross(v3f_t u, v3f_t v);

f32_t V3_len(v3f_t v);

void V3_normalize(v3f_t *v);

v3f_t V3_normalized(v3f_t v);

v2f_t V2_add(v2f_t a, v2f_t b);

v2f_t V2_sub(v2f_t a, v2f_t b);

v2f_t V2_scale(v2f_t a, f32_t s);

f32_t V2_len(v2f_t v);

v2f_t V2_lerp(f32_t lerp, v2f_t a, v2f_t b);
v3f_t V3_lerp(f32_t lerp, v3f_t a, v3f_t b);
v4f_t V4_lerp(f32_t lerp, v4f_t a, v4f_t b);

v2f_t V2_blend(v2f_t a, v2f_t b);
v3f_t V3_blend(v3f_t a, v3f_t b);
v4f_t V4_blend(v4f_t a, v4f_t b);

static inline v2f_t V2(f32_t x, f32_t y)
{
	return (v2f_t){x, y};
}
static inline v2i_t V2i(i32_t x, i32_t y)
{
	return (v2i_t){x, y};
}
static inline v3f_t V3(f32_t x, f32_t y, f32_t z)
{
	return (v3f_t){x, y, z};
}
static inline v4f_t V4(f32_t x, f32_t y, f32_t z, f32_t w)
{
	return (v4f_t){x, y, z, w};
}
static inline v3f_t V2_to_V3(v2f_t v, f32_t z)
{
	return (v3f_t){v.x, v.y, z};
}
static inline v4f_t V3_to_V4(v3f_t v, f32_t w)
{
	return (v4f_t){v.x, v.y, v.z, w};
}

v3f_t V3_proj_point_line(v3f_t p1, v3f_t p2, v3f_t p);
f64_t remap(f64_t val, f64_t a1, f64_t a2, f64_t b1, f64_t b2);
v3f_t orthagonal_project(v3f_t normal, v3f_t u);
void V3_print(v3f_t v);
v3f_t V3_raycast_plane(v3f_t normal, v3f_t pos, v3f_t p, v3f_t dir);
bool triangle_contains(v3f_t *trig, v3f_t p);
v3f_t V3_normal(v3f_t a, v3f_t b, v3f_t c);
#endif /* _VECTOR_H_ */