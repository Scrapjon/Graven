#ifndef SHADER_SOURCES_H
#define SHADER_SOURCES_H

/**
 * !! THIS SHOULD BE AUTOMATICALLY GENERATED IN FUTURE !!
 * It Should Contain All Shaders As Statics
 */

namespace Shaders
{
	namespace Default
	{
		static const char *vert =
			"attribute vec3 a_position;\n"
			"attribute vec2 a_texcoord;\n"

			"uniform mat4 u_mvp;\n"
			"varying vec2 v_texcoord;\n"

			"void main() {\n"
			"	gl_Position = u_mvp * vec4(a_position, 1.0);\n"
			"	v_texcoord = a_texcoord;\n"
			"}\n";
		static const char *frag =
			"varying vec2 v_texcoord;\n"

			"uniform sampler2D u_texture;\n"

			"void main() {\n"
			"	gl_FragColor = texture2D(u_texture, v_texcoord);\n"
			"}\n";
	} // namespace Default

} // namespace Shaders

#endif /* SHADER_SOURCES_H */