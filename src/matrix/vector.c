/*==========================================================*
 *
 * @author Gustaf Franzén :: https://github.com/BjorneEk;
 *
 * vector functions
 *
 *==========================================================*/

#include "vector.h"
#include <math.h>
#include <stdio.h>
v3f_t V3_sub(v3f_t a, v3f_t b)
{
	return (v3f_t) {a.x - b.x, a.y - b.y, a.z - b.z};
}

v3f_t V3_add(v3f_t a, v3f_t b)
{
	return (v3f_t) {a.x + b.x, a.y + b.y, a.z + b.z};
}
v3f_t V3_scale(v3f_t a, f32_t s)
{
	return (v3f_t) {a.x * s, a.y * s, a.z * s};
}

f32_t V3_dot(v3f_t u, v3f_t v)
{
	return u.x * v.x + u.y * v.y + u.z * v.z;
}

v3f_t V3_cross(v3f_t u, v3f_t v)
{
	return(v3f_t) {
		u.y * v.z - u.z * v.y,
		u.z * v.x - u.x * v.z,
		u.x * v.y - u.y * v.x
	};
}

f32_t V3_len(v3f_t v)
{
	return sqrtf(fabsf(v.x * v.x) + fabsf(v.y * v.y) + fabsf(v.z * v.z));
}

void V3_normalize(v3f_t *v)
{
	f32_t l;
	l = V3_len(*v);
	v->x /= l, v->y /= l, v->z /= l;
}

v3f_t V3_normalized(v3f_t v)
{
	f32_t l;
	l = V3_len(v);

	return (v3f_t) {
		v.x / l,
		v.y / l,
		v.z / l
	};
}

v2f_t V2_add(v2f_t a, v2f_t b)
{
	return (v2f_t){a.x + b.x, a.y + b.y};
}

v2f_t V2_sub(v2f_t a, v2f_t b)
{
	return (v2f_t){a.x - b.x, a.y - b.y};
}

v2f_t V2_scale(v2f_t a, f32_t s)
{
	return (v2f_t){a.x * s, a.y * s};
}

f64_t remap(f64_t val, f64_t a1, f64_t a2, f64_t b1, f64_t b2)
{
	return (b1) + (val - a1) * (b2 - b1) / (a2 - a1);
}
v2f_t V2_lerp(f32_t lerp, v2f_t a, v2f_t b)
{
	return V2(
		remap(lerp, 0.0, 1.0, a.x, b.x),
		remap(lerp, 0.0, 1.0, a.y, b.y));
}
v3f_t V3_lerp(f32_t lerp, v3f_t a, v3f_t b)
{
	return V3(
		remap(lerp, 0.0, 1.0, a.x, b.x),
		remap(lerp, 0.0, 1.0, a.y, b.y),
		remap(lerp, 0.0, 1.0, a.z, b.z));
}
v4f_t V4_lerp(f32_t lerp, v4f_t a, v4f_t b)
{
	return V4(
		remap(lerp, 0.0, 1.0, a.x, b.x),
		remap(lerp, 0.0, 1.0, a.y, b.y),
		remap(lerp, 0.0, 1.0, a.z, b.z),
		remap(lerp, 0.0, 1.0, a.w, b.w));
}

v2f_t V2_blend(v2f_t a, v2f_t b)
{
	return V2_lerp(0.5, a, b);
}
v3f_t V3_blend(v3f_t a, v3f_t b)
{
	return V3_lerp(0.5, a, b);
}
v4f_t V4_blend(v4f_t a, v4f_t b)
{
	return V4_lerp(0.5, a, b);
}

f32_t V2_len(v2f_t v)
{
	return sqrtf(fabsf(v.x * v.x + v.y * v.y));
}

v3f_t V3_proj_point_line(v3f_t p1, v3f_t p2, v3f_t p)
{
	f32_t dist;
	v3f_t dir;
	v3f_t new_p2;

	dir = V3_normalized(V3_sub(p1, p2));
	dist =  V3_dot(dir, V3_sub(p, p1));

	return V3_add(V3_scale(dir, dist), p1);
}
void V3_print(v3f_t v)
{
	printf("V3(%.10f, %.10f, %.10f)", v.x, v.y, v.z);
}

v3f_t V3_raycast_plane(v3f_t normal, v3f_t pos, v3f_t p, v3f_t dir)
{
	f64_t d;
	f64_t t;

	d = V3_dot(pos, V3_scale(normal, -1.0f));
	t = -(d + V3_dot(p, normal)) / V3_dot(dir, normal);
	return V3_add(p, V3_scale(dir, t));
}

bool triangle_contains(v3f_t *trig, v3f_t p)
{
	f32_t a, b, c, d, e, ac_bb;
	f32_t x, y, z;
	v3f_t e10, e20, vp;

	e10= V3_sub(trig[1],trig[0]);
	e20= V3_sub(trig[2],trig[0]);

	a = V3_dot(e10, e10);
	b = V3_dot(e10, e20);
	c = V3_dot(e20, e20);
	ac_bb = (a * c) - (b * b);
	vp = V3(p.x - trig[0].x, p.y - trig[0].y, p.z - trig[0].z);

	d = V3_dot(vp, e10);
	e = V3_dot(vp, e20);
	x = (d * c) - (e * b);
	y = (e * a) - (d * b);
	z = x + y - ac_bb;
	return z < 0 && x >= 0 && y >= 0;
}
v3f_t V3_normal(v3f_t a, v3f_t b, v3f_t c)
{
	return V3_cross(V3_sub(b, a), V3_sub(c, a));
}

v3f_t orthagonal_project(v3f_t normal, v3f_t u)
{
	v3f_t u_;

	u_ = V3_scale(normal, V3_dot(normal, u));

	return V3_sub(u, u_);
}