
#version 330 core

layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec3 in_normal;

out vec3 normal;
out vec3 frag_pos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main()
{
	frag_pos = vec3(model * vec4(in_pos, 1.0));

	//frag_pos = vec3(model * vec4(in_pos, 1.0));
	normal = mat3(transpose(inverse(model))) * in_normal;
	gl_Position = projection * view * model * vec4(in_pos, 1.0);
	//gl_Position = projection * view * model * vec4(frag_pos, 1.0);
}