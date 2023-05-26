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
uniform float u_Ns; // Shininess

uniform int u_have_normal_texture;

uniform vec3 u_Ka;
uniform vec3 u_Kd;
uniform vec3 u_Ks;

uniform vec4 u_color;
uniform sampler2D u_texture;
uniform sampler2D u_texture_normal;
uniform float u_time;

out vec4 FragColor;

mat3 cotangent_frame(vec3 N, vec3 p, vec2 uv)
{
	// get edge vectors of the pixel triangle
	vec3 dp1 = dFdx(p);
	vec3 dp2 = dFdy(p);
	vec2 duv1 = dFdx(uv);
	vec2 duv2 = dFdy(uv);
	
	// solve the linear system
	vec3 dp2perp = cross(dp2, N);
	vec3 dp1perp = cross(N, dp1);
	vec3 T = dp2perp * duv1.x + dp1perp * duv2.x;
	vec3 B = dp2perp * duv1.y + dp1perp * duv2.y;
 
	// construct a scale-invariant frame 
	float invmax = inversesqrt(max(dot(T,T), dot(B,B)));
	return mat3(T * invmax, B * invmax, N);
}

// assume N, the interpolated vertex normal and 
// WP the world position
//vec3 normal_pixel = texture2D(normalmap, uv).xyz; 
vec3 perturbNormal(vec3 N, vec3 WP, vec2 uv, vec3 normal_pixel)
{
	normal_pixel = normal_pixel * 255./127. - 128./127.;
	mat3 TBN = cotangent_frame(N, WP, uv);
	return normalize(TBN * normal_pixel);
}

void main()
{
	vec2 uv = v_uv;
	vec4 color;

	vec3 N;
	if(u_have_normal_texture == 1){
		vec3 normal_pixel = texture2D(u_texture_normal, v_uv).xyz;
		N = normalize(perturbNormal(v_normal, v_world_position, v_uv, normal_pixel));
	}
	if(u_have_normal_texture == 0){
		N = normalize(v_normal);
	}
	vec3 L = normalize(lightPos - v_position);
	// Lambert's cosine law
	float lambertian = max(dot(N, L), 0.0);
	float specular = 0.0;
	if(lambertian > 0.0) {
		vec3 R = reflect(-L, N);
		vec3 V = normalize(-v_position);
	    float specAngle = max(dot(R, V), 0.0);
		specular = pow(specAngle, u_Ns);
	}
	color = vec4(Ia * u_Ka +
               Kd * lambertian * u_Kd +
               Ks * specular * u_Ks, 1.0);
	color *= texture(u_texture, v_uv);
	FragColor = color;
}