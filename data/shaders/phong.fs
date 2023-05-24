#version 330

varying vec3 v_position;
varying vec3 v_world_position;
varying vec3 v_normal;
varying vec2 v_uv;
varying vec4 v_color;

uniform vec3 lightPos;
uniform float Ia;   // Ambient reflection coefficient
uniform float Kd;   // Diffuse reflection coefficient
uniform float Ks;   // Specular reflection coefficient
uniform float shininessVal; // Shininess

uniform vec3 u_Ka;
uniform vec3 u_Kd;
uniform vec3 u_Ks;

uniform vec4 u_color;
uniform sampler2D u_texture;
uniform float u_time;

out vec4 FragColor;

void main()
{
	vec2 uv = v_uv;
	vec4 color;

	vec3 N = normalize(v_normal);
	vec3 L = normalize(lightPos - v_position);
	// Lambert's cosine law
	float lambertian = max(dot(N, L), 0.0);
	float specular = 0.0;
	if(lambertian > 0.0) {
		vec3 R = reflect(-L, N);
		vec3 V = normalize(-v_position);
	    float specAngle = max(dot(R, V), 0.0);
		specular = pow(specAngle, shininessVal);
	}
	color = vec4(Ia * u_Ka +
               Kd * lambertian * u_Kd +
               Ks * specular * u_Ks, 1.0);
	FragColor = color;
}