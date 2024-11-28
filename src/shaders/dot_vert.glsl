#version 330 core
layout (location = 0) in vec2 vertex;

uniform mat4 projection;
uniform mat4 rot;
uniform mat4 view;
uniform vec3 pos;
uniform float radius;

void main()
{
	vec4 vert = (rot * vec4(radius * vertex, 0.0, 1.0)) + vec4(pos, 1.0);
	gl_Position = projection * view * vec4(vert.xyz, 1.0);
}