/*==========================================================*
 *
 * @author Gustaf Franzén :: https://github.com/BjorneEk;
 *
 * gl object
 *
 *==========================================================*/

#ifndef _GL_OBJECT_H_
#define _GL_OBJECT_H_





#include "../util/types.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>


typedef struct globject {
	u32_t VAO;
	u32_t VBO;
	u32_t length;
} GL_object_t;

#endif /* _GL_OBJECT_H_ */