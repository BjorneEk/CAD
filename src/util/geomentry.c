

#include "geometry.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

v2f_t circle_cord(f32_t rad, f32_t angle)
{
	return V2((cos(angle) * rad), (sin(angle) * rad));
}

f32_t *gen_sector(v2f_t org, f32_t radius, f32_t start_angle, f32_t span_angle, u32_t triangle_count)
{
	f32_t *res;
	f32_t angle;
	f32_t step;
	i32_t i;

	v2f_t p1, p2;

	res = malloc(triangle_count * 6 * sizeof (f32_t));

	step = span_angle / triangle_count;
	angle = start_angle;

	for (i = 0; i < triangle_count * 6; i += 6) {
		res[i] = org.x;
		res[i + 1] = org.y;

		p1 = V2_add(circle_cord(radius, angle), org);
		angle += step;
		p2 = V2_add(circle_cord(radius, angle), org);

		res[i + 2] = p1.x;
		res[i + 3] = p1.y;
		res[i + 4] = p2.x;
		res[i + 5] = p2.y;
	}
	return res;
}

void append_rect(f32_t *verts, v2f_t org, f32_t width, f32_t height)
{
	verts[0] = 	org.x;
	verts[1] = 	org.y + height;
	verts[2] = 	org.x;
	verts[3] = 	org.y;
	verts[4] = 	org.x + width;
	verts[5] = 	org.y;
	verts[6] = 	org.x;
	verts[7] = 	org.y + height;
	verts[8] = 	org.x + width;
	verts[9] = 	org.y;
	verts[10] = 	org.x + width;
	verts[11] = 	org.y + height;
}
static inline bool inside_rect(v2f_t org, f32_t width, f32_t height, v2f_t pos)
{
	return pos.x >= org.x && pos.x <= org.x + width && pos.y >= org.y && pos.y <= org.y + height;
}
static inline bool inside_circle(v2f_t org, f32_t radius, v2f_t pos)
{
	return V2_len(V2_sub(org, pos)) <= radius;
}

bool rounded_rect_contains(v2f_t pos, f32_t width, f32_t height, f32_t radius, v2f_t p)
{
	bool bl, tl, br, tr;
	bool res;

	if (radius == 0)
		return inside_rect(pos, width, height, p);

	bl = inside_circle(V2_add(pos, V2(radius, radius)), radius, p);

	if (2.0 * radius == height && height == width)
		return inside_circle(V2_add(pos, V2(radius, radius)), radius, p);

	br = inside_circle(V2_add(pos, V2(width - radius, radius)), radius, p);
	tl = inside_circle(V2_add(pos, V2(radius, height - radius)), radius, p);
	if (2.0 * radius == height) {
		return inside_rect(V2_add(pos, V2(radius, 0.0)), width - height, height, p) // 2 * c->radius == c->height
		|| (p.x < pos.x + radius && bl)
		|| (p.x > pos.x + width - radius && br);
	} else if (2.0 * radius == width) {
		return inside_rect(V2_add(pos, V2(0.0, radius)), width, height - width, p) // 2 * c->radius == c->width
		|| (p.y < pos.y + radius && bl)
		|| (p.y > pos.y + height - radius && tl);
	}

	tr = inside_circle(V2_add(pos, V2(width - radius, height - radius)), radius, p);

	res = inside_rect(V2_add(pos, V2(radius, 0.0)), width - 2 * radius, height, p)		// center rect
	|| inside_rect(V2_add(pos, V2(0.0, radius)), radius, height - 2 * radius, p)		// left rect
	|| inside_rect(V2_add(pos, V2(width - radius, radius)), radius, height - 2 * radius, p)	// right rect
	|| (p.x < pos.x + radius && p.y < pos.y + radius && bl)					// bottom left radius
	|| (p.x < pos.x + radius && p.y > pos.y + height - radius && tl)			// top left radius
	|| (p.x > pos.x + width - radius && p.y < pos.y + radius && br)				// bottom right radius
	|| (p.x > pos.x + width - radius && p.y > pos.y + height - radius && tr);		// top right radius
	return res;
}

f32_t *rounded_rect(v2f_t org, f32_t width, f32_t height, f32_t radius, u32_t *floats)
{
	f32_t *res;
	f32_t *sector1;
	f32_t *sector2;
	f32_t *sector3;
	f32_t *sector4;

	if (radius == 0) {
		res = malloc(6*2 * sizeof(f32_t));
		append_rect(res, org, width, height);
		*floats = 6*2;
		return res;
	} else if (2.0 * radius == height && height == width) {
		*floats = 40*6;
		return gen_sector(V2_add(org, V2(radius, radius)), radius, 0, M_PI * 2, 40);
	} else if (2.0 * radius == height) {
		sector1 = gen_sector(V2_add(org, V2(radius, radius)), radius, M_PI/2, M_PI, 20);
		sector2 = gen_sector(V2_add(org, V2(width - radius, radius)), radius, -M_PI / 2, M_PI, 20);
		res = malloc(252 * sizeof(f32_t));
		memcpy(res, sector1, 120 * sizeof(f32_t));
		append_rect(res + 120, V2_add(org, V2(radius, 0)), width - height, height);
		memcpy(res + 132, sector2, 120 * sizeof(f32_t));
		*floats = 252;
		free(sector1);
		free(sector2);
		return (f32_t*)res;
	} else if (2.0 * radius == width) {
		sector1 = gen_sector(V2_add(org, V2(radius, radius)), radius, M_PI, M_PI, 20);
		sector2 = gen_sector(V2_add(org, V2(radius, height - radius)), radius, 0, M_PI, 20);
		res = malloc(252 * sizeof(f32_t));
		memcpy(res, sector1, 120 * sizeof(f32_t));
		append_rect(res + 120, V2_add(org, V2(0, radius)), width, height - width);
		memcpy(res + 132, sector2, 120 * sizeof(f32_t));
		*floats = 252;
		free(sector1);
		free(sector2);
		return (f32_t*)res;
	}
	sector1 = gen_sector(V2_add(org, V2(radius, radius)), radius, M_PI, M_PI / 2, 10);
	sector2 = gen_sector(V2_add(org, V2(radius, height - radius)), radius, M_PI / 2.0, M_PI / 2, 10);
	sector3 = gen_sector(V2_add(org, V2(width - radius, radius)), radius, -M_PI / 2.0, M_PI / 2, 10);
	sector4 = gen_sector(V2_add(org, V2(width - radius, height - radius)), radius, 0, M_PI / 2, 10);

	res = malloc(276 * sizeof(f32_t));
	memcpy(res, sector1, 60 * sizeof(f32_t));
	append_rect(res + 60, V2_add(org, V2(0, radius)), radius, height - 2 * radius);
	memcpy(res + 72, sector2, 60 * sizeof(f32_t));
	append_rect(res + 132, V2_add(org, V2(radius, 0)), width-2*radius, height);
	memcpy(res + 144, sector3, 60 * sizeof(f32_t));
	append_rect(res + 204, V2_add(org, V2(width-radius, radius)), radius, height - 2 * radius);
	memcpy(res + 216, sector4, 60 * sizeof(f32_t));

	*floats = 276;
	free(sector1);
	free(sector2);
	free(sector3);
	free(sector4);
	return (f32_t*)res;
}