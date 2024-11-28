#version 330 core

out vec4 out_color;
uniform vec4 color;

void main()
{
	out_color = color;
	//color = vec4(1.0,1.0,1.0,1.0);//vec4(text_color, 1.0);
}