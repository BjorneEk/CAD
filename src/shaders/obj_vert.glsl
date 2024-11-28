#version 330 core
layout (location = 0) in vec3 vertex;

uniform mat4 projection;
uniform vec3 normal;
uniform vec3 pos;
uniform mat4 view;
uniform mat4 rotation;

out vec3 out_normal;
out vec3 frag_pos;

void main()
{
	out_normal = (rotation * vec4(normal.xyz, 1.0)).xyz;
	//out_normal = (view * vec4(normal.xyz, 1.0)).xyz;
	gl_Position = projection * view * vec4(pos + vertex.xyz, 1.0);
}