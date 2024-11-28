/*==========================================================*
 *
 * @author Gustaf Franzén :: https://github.com/BjorneEk;
 *
 * opengl utility functions
 *
 *==========================================================*/


#include "glutils.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>


GL_object_t V3_VAO(f32_t *object, u32_t size)
{
	GL_object_t res;
	glGenVertexArrays(1, &res.VAO);
	glGenBuffers(1, &res.VBO);

	glBindVertexArray(res.VAO);

	glBindBuffer(GL_ARRAY_BUFFER, res.VBO);
	glBufferData(GL_ARRAY_BUFFER, size * sizeof(f32_t), object, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(f32_t), (void*)0);
	glEnableVertexAttribArray(0);
	res.length = size;
	return res;
}

GL_object_t V2_VAO(f32_t *object, u32_t size)
{
	GL_object_t res;
	glGenVertexArrays(1, &res.VAO);
	glGenBuffers(1, &res.VBO);

	glBindVertexArray(res.VAO);

	glBindBuffer(GL_ARRAY_BUFFER, res.VBO);
	glBufferData(GL_ARRAY_BUFFER, size * sizeof(f32_t), object, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(f32_t), (void*)0);
	glEnableVertexAttribArray(0);
	res.length = size;
	return res;
}

GL_object_t V3_normal_VAO(f32_t *object, u32_t size)
{
	GL_object_t res;
	glGenVertexArrays(1, &res.VAO);
	glGenBuffers(1, &res.VBO);

	glBindVertexArray(res.VAO);

	glBindBuffer(GL_ARRAY_BUFFER, res.VBO);
	glBufferData(GL_ARRAY_BUFFER, size * sizeof(f32_t), object, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(f32_t), (void*)0);
	glEnableVertexAttribArray(0);
	// normal attribnute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(f32_t), (void*)(3 * sizeof(f32_t)));
	glEnableVertexAttribArray(1);
	res.length = size;
	return res;
}