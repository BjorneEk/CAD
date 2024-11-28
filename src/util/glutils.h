/*==========================================================*
 *
 * @author Gustaf Franzén :: https://github.com/BjorneEk;
 *
 * opengl utility functions
 *
 *==========================================================*/

#ifndef _GLUTILS_H_
#define _GLUTILS_H_

#include "types.h"
#include "../GL/globject.h"

GL_object_t V3_VAO(f32_t *object, u32_t size);
GL_object_t V2_VAO(f32_t *object, u32_t size);
GL_object_t V3_normal_VAO(f32_t *object, u32_t size);
#endif /* _GLUTILS_H_ */