attribute vec3 a_position;
attribute vec2 a_texcoord;

uniform mat4 u_mvp;
uniform float u_time;
varying vec2 v_texcoord;

void main() {
	
	vec3 pos = a_position;
	pos.y += sin(pos.x * 0.1 + u_time * 2.0) * 4.0 + cos(pos.z * 0.1 + u_time * 1.5) * 4.0;
	
	gl_Position = u_mvp * vec4(pos, 1.0);
	
	v_texcoord = a_texcoord + vec2(sin(u_time + pos.z * 0.05) * 0.02, cos(u_time + pos.x * 0.05) * 0.02);
	
}