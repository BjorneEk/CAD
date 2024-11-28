


#include "line.h"
#include "../matrix/matrix.h"
#include "../matrix/vector.h"

#include <stdlib.h>
#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <math.h>


f32_t line_point_dist(line_t l, v3f_t p)
{
	return ((l.p2.x - l.p1.x)*(l.p1.y - p.y) - (l.p2.y - l.p1.y)*(l.p1.x - p.x))
	/ sqrt(((l.p2.x - l.p1.x)*(l.p2.x - l.p1.x)+(l.p2.y - l.p1.y)*(l.p2.y - l.p1.y)));
}

v3f_t line_normal(line_t l)
{
	return V3_normalized(V3(l.p1.x - l.p2.x, l.p1.y - l.p2.y, l.p1.z - l.p2.z));
}

v3f_t line_orthagonal_project_point(line_t l, v3f_t p)
{
	f32_t dist;
	v3f_t dir;
	v3f_t new_p2;

	dir	= V3_normalized(V3_sub(l.p1, l.p2));
	dist	= V3_dot(dir, V3_sub(p, l.p1));

	return V3_add(V3_scale(dir, dist), l.p1);
}

bool is_on_line(line_t l, v3f_t p)
{
	f32_t dx, dy;
	dx = l.p2.x - l.p1.x;
	dy = l.p2.y - l.p1.y;
	if (fabs(dx) >= fabs(dy))
		return dx > 0 ?
		l.p1.x <= p.x && p.x <= l.p2.x :
		l.p2.x <= p.x && p.x <= l.p1.x;
	else
		return dy > 0 ?
			l.p1.y <= p.y && p.y <= l.p2.y :
			l.p2.y <= p.y && p.y <= l.p1.y;
}

static bool safe_float_equals(f32_t a, f32_t b)
{
	static const f32_t accuracy = 0.0001;
	return a + accuracy >= b && a - accuracy <= b;
}

v3f_t round_v3(v3f_t v, u32_t decimals)
{
	f32_t fac;


	fac = pow(10, decimals);
	return (v3f_t) {
		(f32_t)((i32_t)(v.x * fac)) / fac,
		(f32_t)((i32_t)(v.y * fac)) / fac,
		(f32_t)((i32_t)(v.z * fac)) / fac
	};
}

bool v3_safe_eq(v3f_t a, v3f_t b)
{
	return safe_float_equals(a.x, b.x) && safe_float_equals(a.y, b.y);
}

bool	line_contains(line_t l, v3f_t p)
{
	return v3_safe_eq(V3_normalized(V3_sub(l.p2, l.p1)), V3_normalized(V3_sub(p, l.p1))) && is_on_line(l, p);
}
f32_t dist_to_line(line_t line, v3f_t p)
{
	return V3_len(V3_sub(V3_proj_point_line(line.p1, line.p2, p), p));
}