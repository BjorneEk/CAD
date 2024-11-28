#version 330 core
layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 in_normal;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 rotation;

out vec3 normal;
out vec3 frag_pos;
void main()
{

	normal = (rotation * vec4(in_normal, 1.0)).xyz;
	//out_normal = (view * vec4(normal.xyz, 1.0)).xyz;
	gl_Position = projection * view * vec4(vertex, 1.0);
}