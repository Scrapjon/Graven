#ifndef SHADER_SOURCES_H
#define SHADER_SOURCES_H

/**
 * AUTO-GENERATED FILE
 * COPYRIGHT MOLONEY
 */

namespace Shaders
{
    
    namespace Default
    {
       static const char *frag = "varying vec2 v_texcoord;uniform sampler2D u_texture;/** This function is very much WIP as I have never used GLSL before lol* - Oliver*/vec4 getAmbientLighting() {vec3 lightColor = vec3(1., 1., 1.); /*temp*/vec3 objectColor = lightColor; /*temp*/float ambientStrength = 1.;   vec3 ambient = ambientStrength * lightColor;   vec3 result = ambient * objectColor;return vec4(result, 1.0);}void main() {   vec4 tex = texture2D(u_texture, v_texcoord);   if(tex.r * 255. == 159. && tex.g * 255. == 91. && tex.b * 255. == 83.) discard;   gl_FragColor = tex * getAmbientLighting();}";
       static const char *vert = "attribute vec3 a_position;attribute vec2 a_texcoord;uniform mat4 u_mvp;varying vec2 v_texcoord;void main() {gl_Position = u_mvp * vec4(a_position, 1.0);v_texcoord = a_texcoord;}";
    } // namespace Default

    namespace Wavy
    {
       static const char *frag = "varying vec2 v_texcoord;uniform sampler2D u_texture;void main() {vec4 tex = texture2D(u_texture, v_texcoord);gl_FragColor = tex;}";
       static const char *vert = "attribute vec3 a_position;attribute vec2 a_texcoord;uniform mat4 u_mvp;uniform float u_time;varying vec2 v_texcoord;void main() {vec3 pos = a_position;pos.y += sin(pos.x * 0.1 + u_time * 2.0) * 4.0 + cos(pos.z * 0.1 + u_time * 1.5) * 4.0;gl_Position = u_mvp * vec4(pos, 1.0);v_texcoord = a_texcoord + vec2(sin(u_time + pos.z * 0.05) * 0.02, cos(u_time + pos.x * 0.05) * 0.02);}";
    } // namespace Wavy

    
} // namespace Shaders

#endif /* SHADER_SOURCES_H */
