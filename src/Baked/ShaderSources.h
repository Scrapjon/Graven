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
       static const char *frag = "varying vec2 v_texcoord;uniform sampler2D u_texture;void main() {   vec4 tex = texture2D(u_texture, v_texcoord);   if(tex.r * 255.f == 159.f && tex.g * 255.f == 91.f && tex.b * 255.f == 83.f) discard;   gl_FragColor = tex;}";
       static const char *vert = "attribute vec3 a_position;attribute vec2 a_texcoord;uniform mat4 u_mvp;varying vec2 v_texcoord;void main() {gl_Position = u_mvp * vec4(a_position, 1.0);v_texcoord = a_texcoord;}";
    } // namespace Default

    
} // namespace Shaders

#endif /* SHADER_SOURCES_H */
