#version 330 core

out vec4 fragment_color;

uniform vec3 color;
uniform vec3 light_pos;


in vec3 normal;
in vec3 frag_pos;

void main()
{/*
	float ambient_strength = 0.1;
	float specular_strength = 0.5;

	vec3 ambient = ambient_strength * color;
	vec3 norm		= normalize(normal);
	float diff = max(dot(norm, vec3(0.0,0.0,-1.0)), 0.0);
	fragment_color = vec4((diff * color), 1.0);
*/

	float ambient_strength = 0.1;
	float specular_strength = 0.5;

	vec3 ambient = ambient_strength * color;

	vec3 norm		= normalize(normal);
	vec3 light_dir		= normalize(light_pos - frag_pos);
	vec3 look_dir		= normalize(vec3(0,0,0) - frag_pos);
	vec3 reflect_dir	= reflect(-light_dir, norm);

	float spec = pow(max(dot(look_dir, reflect_dir), 0.0), 128);
	vec3 specular = vec3(1,1,1)*specular_strength * spec;

	float diff = max(dot(norm, light_dir), 0.0);

	vec3 result = (ambient + diff + specular) * color;
	fragment_color = vec4(result, 1.0);

}