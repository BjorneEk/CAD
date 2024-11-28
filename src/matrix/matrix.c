/*==========================================================*
 *
 * @author Gustaf Franzén :: https://github.com/BjorneEk;
 *
 * 3x3 and 4x4 matrix library for graphics
 *
 *==========================================================*/

#include "matrix.h"
#include "vector.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#define IDX(_mat, _x, _y) (_mat)[(_y) + (_x) * 4]


f32_t *M4_new()
{
	return (f32_t *) calloc(16, sizeof(f32_t));
}

f32_t *M4_from(f32_t from[16])
{
	f32_t *res;

	res = M4_new();

	memcpy(res, from, 16 * sizeof(f32_t));
	return res;
}

void M4_clear(f32_t *m)
{
	memset(m, 0, 16 * sizeof(f32_t));
}

void M4_set_diagonal(f32_t *m, f32_t a, f32_t b, f32_t c, f32_t d)
{
	IDX(m, 0, 0) = a;
	IDX(m, 1, 1) = b;
	IDX(m, 2, 2) = c;
	IDX(m, 3, 3) = d;
}

f32_t	*M4_diag(f32_t a, f32_t b, f32_t c, f32_t d)
{
	return M4_from((f32_t[16]) {
		a, 0, 0, 0,
		0, b, 0, 0,
		0, 0, c, 0,
		0, 0, 0, d
	});
}


f32_t	*M4_eye()
{
	return M4_diag(1, 1, 1, 1);
}

f32_t *M4_copy(f32_t *m)
{
	f32_t *res;

	res = M4_new();

	memcpy(res, m, 16 * sizeof(f32_t));
	return res;
}

void M4_transpose(f32_t *m)
{
	f32_t tmp;
	i32_t i, j;

	for (i = 0; i < 4; ++i) {
		for (j = i + 1; j < 4; ++j) {
			tmp = IDX(m, i, j);
			IDX(m, i, j) = IDX(m, j, i);
			IDX(m, j, i) = tmp;
		}
	}
}

f32_t *M4_transposed(f32_t *m)
{
	f32_t *res;

	res = M4_copy(m);
	M4_transpose(res);
	return res;
}

/**
 * print the matrix to stdout
 */
void M4_print(f32_t *m)
{
	i32_t i, j;

	for (i = 0; i < 4; ++i) {
		for (j = 0; j < 3; ++j) {
			printf("%.1f	", IDX(m, j, i));
		}
		printf("%.3f\n", IDX(m, j, i));
	}
}

void M4_add(f32_t *a, f32_t *b)
{
	i32_t i;

	for (i = 0; i < 16; i++)
		a[i] += b[i];
}

void M4_sub(f32_t *a, f32_t *b)
{
	i32_t i;

	for (i = 0; i < 16; i++)
		a[i] -= b[i];
}

void M4_mult(f32_t *a, f32_t *b)
{
	i32_t c, r, i;
	f32_t val;
	f32_t tmp[16];

	for (c = 0; c < 4; c++)
		for (r = 0; r < 4; r++) {
			val = 0.0f;
			for (i = 0; i < 4; i++)
				val += IDX(a, i, c) * IDX(b, r, i);
			IDX(tmp, r, c) = val;
		}
	memcpy(a, tmp, 16 * sizeof(f32_t));
}

// v3f_t M4_mult_V3(f32_t *m, v3f_t v)
// {
// 	v3f_t res;

// 	res.x = m[0] * v.x + m[4] * v.y + m[8] * v.z + m[12];
// 	res.y = m[1] * v.x + m[5] * v.y + m[9] * v.z + m[13];
// 	res.z = m[2] * v.x + m[6] * v.y + m[10] * v.z + m[14];

// 	return res;
// }
v3f_t M4_mult_V3(f32_t *m, v3f_t v)
{
	v3f_t res;
	f32_t w;
	res.x	= m[0] * v.x + m[4] * v.y + m[8] * v.z + m[12];
	res.y	= m[1] * v.x + m[5] * v.y + m[9] * v.z + m[13];
	res.z	= m[2] * v.x + m[6] * v.y + m[10] * v.z + m[14];
	w	= m[3] * v.x + m[7] * v.y + m[11] * v.z + m[15];
	if (w != 0) {
		res.x /= w;
		res.y /= w;
		res.z /= w;
	}
	return res;
}
f32_t *M4_rotation(v3f_t v, f32_t rad)
{
	f32_t *res;
	f32_t c, s;
	f32_t x2, y2, z2;
	f32_t xy, xz, yz;

	res = M4_new();
	c = cos(rad);
	s = sin(rad);

	V3_normalize(&v);

	x2 = v.x * v.x;
	y2 = v.y * v.y;
	z2 = v.z * v.z;
	xy = v.x * v.y;
	xz = v.x * v.z;
	yz = v.y * v.z;

	res[0] = c + x2 * (1 - c);
	res[1] = xy * (1 - c) + v.z * s;
	res[2] = xz * (1 - c) - v.y * s;
	res[3] = 0.0f;

	res[4] = xy * (1 - c) - v.z * s;
	res[5] = c + y2 * (1 - c);
	res[6] = yz * (1 - c) + v.x * s;
	res[7] = 0.0f;

	res[8] = xz * (1 - c) + v.y * s;
	res[9] = yz * (1 - c) - v.x * s;
	res[10] = c + z2 * (1 - c);
	res[11] = 0.0f;

	res[12] = 0.0f;
	res[13] = 0.0f;
	res[14] = 0.0f;
	res[15] = 1.0f;

	return res;
}

f32_t *M4_translation(v3f_t v)
{
	return M4_from((f32_t[16]) {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		v.x, v.y, v.z, 1
	});
}

void M4_rotate(f32_t **m, v3f_t v, f32_t rad)
{
	f32_t *rot;

	rot = M4_rotation(v, rad);

	M4_mult(rot, *m);
	free(*m);
	*m = rot;
}

void M4_translate(f32_t **m, v3f_t v)
{
	f32_t *trans;

	trans = M4_translation(v);
	M4_mult(trans, *m);
	free(*m);
	*m = trans;
}

void M4_scale(f32_t **m, v3f_t v)
{
	f32_t *scale;

	scale = M4_from((f32_t[16]) {
		v.x, 0, 0, 0,
		0, v.y, 0, 0,
		0, 0, v.z, 0,
		0, 0, 0, 1
	});

	M4_mult(scale, *m);
	free(*m);
	*m = scale;
}

f32_t *M4_perspective(f32_t fov, f32_t ar, f32_t n, f32_t f)
{
	f32_t * res;
	f32_t tan_half_FOV;
	f32_t range_inv;

	res = M4_new();
	tan_half_FOV = tan(fov / 2.0f);
	range_inv = 1.0f / (n - f);

	res[0] = 1.0f / (tan_half_FOV * ar);
	res[1] = 0.0f;
	res[2] = 0.0f;
	res[3] = 0.0f;

	res[4] = 0.0f;
	res[5] = 1.0f / tan_half_FOV;
	res[6] = 0.0f;
	res[7] = 0.0f;

	res[8] = 0.0f;
	res[9] = 0.0f;
	res[10] = (f + n) * range_inv;
	res[11] = -1.0f;

	res[12] = 0.0f;
	res[13] = 0.0f;
	res[14] = 2.0f * f * n * range_inv;
	res[15] = 0.0f;

	return res;
}
f32_t *M4_inverse_perspective(f32_t fov, f32_t ar, f32_t n, f32_t f)
{
	f32_t * res;
	f32_t tan_half_FOV;
	f32_t range_inv;

	res = M4_new();
	tan_half_FOV = tan(fov * 0.5);
	range_inv = 1.0f / 2.0f * n * f;

	res[0] = tan_half_FOV * ar;
	res[1] = 0.0f;
	res[2] = 0.0f;
	res[3] = 0.0f;

	res[4] = 0.0f;
	res[5] = tan_half_FOV;
	res[6] = 0.0f;
	res[7] = 0.0f;

	res[8] = 0.0f;
	res[9] = 0.0f;
	res[10] = 0.0f;
	res[11] = (n - f) * range_inv;

	res[12] = 0.0f;
	res[13] = 0.0f;
	res[14] = -1.0f;
	res[15] = (n + f) * range_inv;

	return res;
}

f32_t *M4_orthographic(f32_t bottom, f32_t top, f32_t left, f32_t right)
{
	f32_t *res;
	res = malloc(sizeof(f32_t) * 16);

	res[0] = 2.0f / (right - left);
	res[1] = 0.0f;
	res[2] = 0.0f;
	res[3] = 0.0f;

	res[4] = 0.0f;
	res[5] = 2.0f / (top - bottom);
	res[6] = 0.0f;
	res[7] = 0.0f;

	res[8] = 0.0f;
	res[9] = 0.0f;
	res[10] = -1.0f;
	//res[10] = -2.0f / (0 - 100);
	res[11] = 0.0f;

	res[12] = -(right + left) / (right - left);
	res[13] = -(top + bottom) / (top - bottom);
	res[14] = 0.0f;
	//res[14] = -1.0;
	res[15] = 1.0f;

	return res;
}
f32_t *M4_inverse_orthographic(f32_t bottom, f32_t top, f32_t left, f32_t right)
{
	f32_t *res;
	res = malloc(sizeof(f32_t) * 16);

	res[0] = (right - left) * 0.5;
	res[1] = 0.0f;
	res[2] = 0.0f;
	res[3] = 0.0f;

	res[4] = 0.0f;
	res[5] = (top - bottom) * 0.5;
	res[6] = 0.0f;
	res[7] = 0.0f;

	res[8] = -(right + left) / 2;
	res[9] = -(top + bottom) / 2;
	//res[10] = -1.0f;
	res[10] = -1.0;
	res[11] = -1.0;

	res[12] = (right + left) / 2;
	res[13] = (top + bottom) / 2;
	//res[14] = 0.0f;
	res[14] = 0.0;
	res[15] = 1.0;

	return res;
}

f32_t *M4_lerp(f32_t *a, f32_t *b, f32_t dist)
{
	i32_t i;
	f32_t *res;

	res = M4_new();

	for(i = 0; i < 16; i++)
		res[i] = remap(dist, 0.0, 1.0, a[i], b[i]);
	return res;
}

f32_t *partial_rotational_transform(v3f_t n, v3f_t target, f32_t step)
{
	v3f_t axis;
	f32_t angle;
	f32_t absn;
	V3_normalize(&n);
	V3_normalize(&target);
	if (fabs(V3_dot(n, target)) == 1.0)
		return M4_eye();

	absn = V3_len(n);
	angle = acos(n.z / absn);
	axis = V3_scale(V3_cross(n, target), 1.0 / absn);
	return M4_rotation(axis, angle * step);
}

f32_t *rotational_transform(v3f_t n, v3f_t target)
{
	return partial_rotational_transform(n, target, 1.0);
}

f32_t *M4_look_at(v3f_t cam, v3f_t target, v3f_t up)
{
	v3f_t cam_dir;
	v3f_t cam_up;
	v3f_t cam_right;
	f32_t *rotation;
	f32_t translation[] = {
		1,	0,	0,	0,
		0,	1,	0,	0,
		0,	0,	1,	0,
		-cam.x,	-cam.y,	-cam.z,	1
	};

	cam_dir = V3_sub(cam, target);
	V3_normalize(&cam_dir);

	cam_right = V3_cross(up, cam_dir);
	V3_normalize(&cam_right);

	cam_up = V3_cross(cam_dir, cam_right);
	V3_normalize(&cam_up);

	rotation = M4_from((f32_t[16]) {
		cam_right.x,	cam_up.x,	cam_dir.x,	0,
		cam_right.y,	cam_up.y,	cam_dir.y,	0,
		cam_right.z,	cam_up.z,	cam_dir.z,	0,
		0,		0,		0,		1
	});
	M4_mult(rotation, translation);
	return rotation;
}

f32_t det_3x3(f32_t a, f32_t b, f32_t c, f32_t d, f32_t e, f32_t f, f32_t g, f32_t h, f32_t i)
{
	return a * (e * i - f * h) - b * (d * i - f * g) + c * (d * h - e * g);
}

f32_t M4_det(f32_t *m)
{
	f32_t det1, det2, det3, det4;

	det1 = det_3x3(m[5], m[6], m[7], m[9], m[10], m[11], m[13], m[14], m[15]);
	det2 = det_3x3(m[4], m[6], m[7], m[8], m[10], m[11], m[12], m[14], m[15]);
	det3 = det_3x3(m[4], m[5], m[7], m[8], m[9], m[11], m[12], m[13], m[15]);
	det4 = det_3x3(m[4], m[5], m[6], m[8], m[9], m[10], m[12], m[13], m[14]);

	return m[0] * det1 - m[1] * det2 + m[2] * det3 - m[3] * det4;
}


void M4_invert_rotation(f32_t *view, f32_t *rotation)
{
	f32_t *rot_inv;

	rot_inv = M4_transposed(rotation);

	M4_mult(view, rot_inv);
	free(rot_inv);
}
void M4_invert_translation(f32_t *view, f32_t *translation)
{
	f32_t *trans_inv;

	trans_inv = M4_copy(translation);
	trans_inv[12] = -trans_inv[12];
	trans_inv[13] = -trans_inv[13];
	trans_inv[14] = -trans_inv[14];

	M4_mult(view, trans_inv);
	free(trans_inv);
}
void M4_invert_scaling(f32_t *view, f32_t *scale)
{
	f32_t *scale_inv;

	scale_inv = M4_copy(scale);
	view[0] *= 1.0 / scale_inv[0];
	view[5] *= 1.0 / scale_inv[5];
	view[10] *= 1.0 / scale_inv[10];

	free(scale_inv);
}

f32_t *M4_invert_model(f32_t *rot, f32_t *trans)
{
	f32_t *rot_inv;

	rot_inv = M4_eye();
	M4_invert_rotation(rot_inv, rot);
	M4_invert_translation(rot_inv, trans);

	return rot_inv;
}

bool M4_invert(f32_t *m, f32_t **result)
{
	f32_t *inv, det;
	i32_t i;

	inv = malloc(16 * sizeof (f32_t));
	inv[0] = m[5]  * m[10] * m[15] -
		m[5]  * m[11] * m[14] -
		m[9]  * m[6]  * m[15] +
		m[9]  * m[7]  * m[14] +
		m[13] * m[6]  * m[11] -
		m[13] * m[7]  * m[10];

	inv[4] = -m[4]  * m[10] * m[15] +
		m[4]  * m[11] * m[14] +
		m[8]  * m[6]  * m[15] -
		m[8]  * m[7]  * m[14] -
		m[12] * m[6]  * m[11] +
		m[12] * m[7]  * m[10];

	inv[8] = m[4]  * m[9] * m[15] -
		m[4]  * m[11] * m[13] -
		m[8]  * m[5] * m[15] +
		m[8]  * m[7] * m[13] +
		m[12] * m[5] * m[11] -
		m[12] * m[7] * m[9];

	inv[12] = -m[4]  * m[9] * m[14] +
		m[4]  * m[10] * m[13] +
		m[8]  * m[5] * m[14] -
		m[8]  * m[6] * m[13] -
		m[12] * m[5] * m[10] +
		m[12] * m[6] * m[9];

	inv[1] = -m[1]  * m[10] * m[15] +
		m[1]  * m[11] * m[14] +
		m[9]  * m[2] * m[15] -
		m[9]  * m[3] * m[14] -
		m[13] * m[2] * m[11] +
		m[13] * m[3] * m[10];

	inv[5] = m[0]  * m[10] * m[15] -
		m[0]  * m[11] * m[14] -
		m[8]  * m[2] * m[15] +
		m[8]  * m[3] * m[14] +
		m[12] * m[2] * m[11] -
		m[12] * m[3] * m[10];

	inv[9] = -m[0]  * m[9] * m[15] +
		m[0]  * m[11] * m[13] +
		m[8]  * m[1] * m[15] -
		m[8]  * m[3] * m[13] -
		m[12] * m[1] * m[11] +
		m[12] * m[3] * m[9];

	inv[13] = m[0]  * m[9] * m[14] -
		m[0]  * m[10] * m[13] -
		m[8]  * m[1] * m[14] +
		m[8]  * m[2] * m[13] +
		m[12] * m[1] * m[10] -
		m[12] * m[2] * m[9];

	inv[2] = m[1]  * m[6] * m[15] -
		m[1]  * m[7] * m[14] -
		m[5]  * m[2] * m[15] +
		m[5]  * m[3] * m[14] +
		m[13] * m[2] * m[7] -
		m[13] * m[3] * m[6];

	inv[6] = -m[0]  * m[6] * m[15] +
		m[0]  * m[7] * m[14] +
		m[4]  * m[2] * m[15] -
		m[4]  * m[3] * m[14] -
		m[12] * m[2] * m[7] +
		m[12] * m[3] * m[6];

	inv[10] = m[0]  * m[5] * m[15] -
		m[0]  * m[7] * m[13] -
		m[4]  * m[1] * m[15] +
		m[4]  * m[3] * m[13] +
		m[12] * m[1] * m[7] -
		m[12] * m[3] * m[5];

	inv[14] = -m[0]  * m[5] * m[14] +
		m[0]  * m[6] * m[13] +
		m[4]  * m[1] * m[14] -
		m[4]  * m[2] * m[13] -
		m[12] * m[1] * m[6] +
		m[12] * m[2] * m[5];

	inv[3] = -m[1] * m[6] * m[11] +
		m[1] * m[7] * m[10] +
		m[5] * m[2] * m[11] -
		m[5] * m[3] * m[10] -
		m[9] * m[2] * m[7] +
		m[9] * m[3] * m[6];

	inv[7] = m[0] * m[6] * m[11] -
		m[0] * m[7] * m[10] -
		m[4] * m[2] * m[11] +
		m[4] * m[3] * m[10] +
		m[8] * m[2] * m[7] -
		 m[8] * m[3] * m[6];

	inv[11] = -m[0] * m[5] * m[11] +
		m[0] * m[7] * m[9] +
		m[4] * m[1] * m[11] -
		m[4] * m[3] * m[9] -
		m[8] * m[1] * m[7] +
		m[8] * m[3] * m[5];

	inv[15] = m[0] * m[5] * m[10] -
		m[0] * m[6] * m[9] -
		m[4] * m[1] * m[10] +
		m[4] * m[2] * m[9] +
		m[8] * m[1] * m[6] -
		m[8] * m[2] * m[5];

	det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

	if (det == 0)
		return false;

	det = 1.0 / det;

	for (i = 0; i < 16; i++)
		inv[i] *= det;

	*result = inv;
	return true;
}