/*==========================================================*
 *
 * @author Gustaf Franzén :: https://github.com/BjorneEk;
 *
 * 3x3 and 4x4 matrix library for graphics
 *
 *==========================================================*/

#ifndef _MATRIX_H_
#define _MATRIX_H_

#include "../util/types.h"
#include "vector.h"
#include <stdlib.h>

/**
 * create a new 4x4 matrix set to 0
 */
f32_t *M4_new();

/**
 * create a new 4x4 matrix with a copy of the supplied matrix.
 * the created matrix is in column major order
 */
f32_t	*M4_from(f32_t from[16]);

f32_t *M4_copy(f32_t *m);

void M4_clear(f32_t *m);

/**
 * sets teh diagonal of @m to the suplied values
 * from top left
 */
void M4_set_diagonal(f32_t *m, f32_t a, f32_t b, f32_t c, f32_t d);

/**
 * create a new 4x4 matrix with the suplied values on the diagonal
 * from top left
 */
f32_t	*M4_diag(f32_t a, f32_t b, f32_t c, f32_t d);

/**
 * create a new 4x4 identity matrix
 */
f32_t	*M4_eye();

/**
 * transpose the matrix
 */
void M4_transpose(f32_t *m);

f32_t *M4_transposed(f32_t *m);

/**
 * print the matrix to stdout
 */
void M4_print(f32_t *m);

/**
 * add b to a element wise
 */
void M4_add(f32_t *a, f32_t *b);

/**
 * sub b from a element wise
 */
void M4_sub(f32_t *a, f32_t *b);

/**
 * multiply a * b store result in a
 */
void M4_mult(f32_t *a, f32_t *b);

v3f_t M4_mult_V3(f32_t *m, v3f_t v);

/**
 * create a rotation matrix for rotation by @rad radians around v
 */
f32_t *M4_rotation(v3f_t v, f32_t rad);

void M4_rotate(f32_t **m, v3f_t v, f32_t rad);

f32_t *M4_translation(v3f_t v);

void M4_translate(f32_t **m, v3f_t v);

/**
 * scale by x y and z
 */
void M4_scale(f32_t **m, v3f_t v);

f32_t *M4_perspective(f32_t fov, f32_t ar, f32_t n, f32_t f);

f32_t *M4_inverse_perspective(f32_t fov, f32_t ar, f32_t n, f32_t f);

f32_t *M4_orthographic(f32_t bottom, f32_t top, f32_t left, f32_t right);

f32_t *M4_inverse_orthographic(f32_t bottom, f32_t top, f32_t left, f32_t right);

f32_t *rotational_transform(v3f_t n, v3f_t target);

f32_t *partial_rotational_transform(v3f_t n, v3f_t target, f32_t step);

f32_t *M4_lerp(f32_t *a, f32_t *b, f32_t dist);

f32_t *M4_look_at(v3f_t cam, v3f_t target, v3f_t up);

f32_t M4_det(f32_t *m);

f32_t *M4_invert_model(f32_t *rot, f32_t *trans);

void M4_invert_rotation(f32_t *view, f32_t *rotation);
void M4_invert_translation(f32_t *view, f32_t *translation);
void M4_invert_scaling(f32_t *view, f32_t *scale);

bool M4_invert(f32_t *m, f32_t **result);

#endif /* _MATRIX_H_ */