#version 130
in vec3 v_world_position;

uniform samplerCube u_texture;
uniform vec3 u_camera_position;

out vec4 FragColor;

void main()
{
	vec3 V = normalize(u_camera_position - v_world_position);
	FragColor = texture(u_texture, V);
}