/*==========================================================*
 *
 * @author Gustaf Franzén :: https://github.com/BjorneEk;
 *
 * sketch representation
 *
 *==========================================================*/

#ifndef _LINE_H_
#define _LINE_H_

#include "../matrix/matrix.h"
#include "../matrix/vector.h"
#include "../GL/shader.h"


typedef struct line {
	v3f_t p1;
	v3f_t p2;
} line_t;


typedef struct sketch_line {
	line_t line;
	bool selected;
} sketch_line_t;


static inline line_t	line(v3f_t p1, v3f_t p2)
{
	return (line_t){p1, p2};
}

f32_t	line_point_dist(line_t l, v3f_t p);
v3f_t	line_normal(line_t l);
v3f_t	line_orthagonal_project_point(line_t l, v3f_t p);
bool	v3_safe_eq(v3f_t a, v3f_t b);
bool	is_on_line(line_t l, v3f_t p);
bool	line_contains(line_t l, v3f_t p);
v3f_t	round_v3(v3f_t v, u32_t decimals);

f32_t dist_to_line(line_t line, v3f_t p);

#endif /* _LINE_H_ */