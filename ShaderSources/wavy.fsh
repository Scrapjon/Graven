varying vec2 v_texcoord;
uniform sampler2D u_texture;

void main() {
	vec4 tex = texture2D(u_texture, v_texcoord);
	gl_FragColor = tex;
}