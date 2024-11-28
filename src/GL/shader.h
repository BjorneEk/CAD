/*==========================================================*
 *
 * @author Gustaf Franzén :: https://github.com/BjorneEk;
 *
 * shader header file
 *
 *==========================================================*/

#ifndef _SHADER_H_
#define _SHADER_H_

#include "../util/types.h"
#include "../matrix/vector.h"
typedef int shader_t;


void shader_load(shader_t * self, const char *vert_path, const char *frag_path);

void shader_use(shader_t * self);

u32_t shader_get_uniform(shader_t * self, const char * name);
void shader_setb(shader_t * self, const char *name, bool value);
void shader_seti(shader_t * self, const char *name, i32_t value);
void shader_setf(shader_t * self, const char *name, f32_t value);

void shader_setM4(shader_t * self, const char *name, f32_t *m);
void shader_setV3(shader_t * self, const char *name, v3f_t v);
void shader_setV4(shader_t * self, const char *name, v4f_t v);
void shader_setV2(shader_t * self, const char *name, v2f_t v);
#endif /* _SHADER_H_ */
