varying vec2 v_texcoord;
uniform sampler2D u_texture;
void main() {
   vec4 tex = texture2D(u_texture, v_texcoord);
   if(tex.r * 255.f == 159.f && tex.g * 255.f == 91.f && tex.b * 255.f == 83.f) discard;
   gl_FragColor = tex;
}