#version 330

varying vec3 v_position;
varying vec3 v_world_position;
varying vec3 v_normal;
varying vec2 v_uv;
varying vec4 v_color;

uniform vec3 u_Kd;
uniform vec4 u_color;
uniform sampler2D u_texture;
uniform float u_time;

void main()
{
	vec2 uv = v_uv;
	vec4 color;
	color.xyz = u_Kd;
	color *= texture(u_texture, v_uv);
	gl_FragColor = color;
}
