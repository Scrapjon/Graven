varying vec2 v_texcoord;
uniform sampler2D u_texture;


uniform vec3 lightColor;
uniform float ambientStrength;

/*
* This function is very much WIP as I have never used GLSL before lol
* - Oliver
*/
vec4 getAmbientLighting() {
	vec3 lightColor = vec3(1.0, 1.0, 1.0);
	vec3 objectColor = lightColor; /*temp*/
	float ambientStrength = 1.0;

   	vec3 ambient = ambientStrength * lightColor;
   	vec3 result = ambient * objectColor;
	return vec4(result, 1.0);
}


void main() {

   	vec4 tex = texture2D(u_texture, v_texcoord);
   	if(tex.r * 255. == 159. && tex.g * 255. == 91. && tex.b * 255. == 83.) discard;

   	gl_FragColor = tex * getAmbientLighting();
}