#version 330 core

in vec3 normal;
out vec4 out_color;

uniform vec4 color;

void main()
{
	// if (normal == vec3(0.0, 0.0, 0.0)) {
	// 	out_color = vec4(1.0, 1.0, 1.0, 1.0);
	// } else {
	out_color = vec4((max(dot(normal, normalize(vec3(0.0, 1.0, 0.0))), 0.2) * color).xyz, 1.0);
	out_color = out_color + max(dot(normal, normalize(vec3(0.0, 0.0, 1.0))), 0.2) * 0.2;
	// }
	//color = vec4(1.0,1.0,1.0,1.0);//vec4(text_color, 1.0);
}