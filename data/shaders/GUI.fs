uniform sampler2D u_texture;
varying vec2 v_uv;

void main()
{
	vec2 charUV = vec2(v_uv.x, 1.0 - v_uv.y);
	vec4 texture = texture2D(u_texture, charUV);
	gl_FragColor = texture;
}
