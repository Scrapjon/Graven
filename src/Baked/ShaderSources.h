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
        static const char *vert = "attribute vec3 a_position;attribute vec2 a_texcoord;uniform mat4 u_mvp;varying vec2 v_texcoord;void main() {	gl_Position = u_mvp * vec4(a_position, 1.0);	v_texcoord = a_texcoord;}";
        static const char *frag = "varying vec2 v_texcoord;uniform sampler2D u_texture;void main() {	gl_FragColor = texture2D(u_texture, v_texcoord);}";
    } // namespace Default
    
} // namespace Shaders

#endif /* SHADER_SOURCES_H */
