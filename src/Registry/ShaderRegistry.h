#ifndef SHADER_REGISTRY_H
#define SHADER_REGISTRY_H

#include <GL/gl.h>
#include <string>
#include <map>
#include <iostream>
#include <cstddef>

#include "Baked/ShaderSources.h"

#include "Types/Vertex.h"

typedef char GLchar;
typedef ptrdiff_t GLsizeiptr;

typedef GLuint(APIENTRY *PFNGLCREATESHADERPROC)(GLenum type);
typedef void(APIENTRY *PFNGLSHADERSOURCEPROC)(GLuint shader, GLsizei count, const GLchar *const *string, const GLint *length);
typedef void(APIENTRY *PFNGLCOMPILESHADERPROC)(GLuint shader);
typedef GLuint(APIENTRY *PFNGLCREATEPROGRAMPROC)(void);
typedef void(APIENTRY *PFNGLATTACHSHADERPROC)(GLuint program, GLuint shader);
typedef void(APIENTRY *PFNGLLINKPROGRAMPROC)(GLuint program);
typedef void(APIENTRY *PFNGLUSEPROGRAMPROC)(GLuint program);
typedef void(APIENTRY *PFNGLGETSHADERIVPROC)(GLuint shader, GLenum pname, GLint *params);
typedef void(APIENTRY *PFNGLGETSHADERINFOLOGPROC)(GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
typedef void(APIENTRY *PFNGLBINDATTRIBLOCATIONPROC)(GLuint program, GLuint index, const GLchar *name);
typedef void(APIENTRY *PFNGLENABLEVERTEXATTRIBARRAYPROC)(GLuint index);
typedef void(APIENTRY *PFNGLDISABLEVERTEXATTRIBARRAYPROC)(GLuint index);
typedef void(APIENTRY *PFNGLVERTEXATTRIBPOINTERPROC)(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer);
typedef GLint(APIENTRY *PFNGLGETUNIFORMLOCATIONPROC)(GLuint program, const GLchar *name);
typedef void(APIENTRY *PFNGLUNIFORM1IPROC)(GLint location, GLint v0);
typedef void(APIENTRY *PFNGLUNIFORMMATRIX4FVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void(APIENTRY *PFNGLGENBUFFERSPROC)(GLsizei n, GLuint *buffers);
typedef void(APIENTRY *PFNGLDELETEBUFFERSPROC)(GLsizei n, const GLuint *buffers);
typedef void(APIENTRY *PFNGLBINDBUFFERPROC)(GLenum target, GLuint buffer);
typedef void(APIENTRY *PFNGLBUFFERDATAPROC)(GLenum target, GLsizeiptr size, const void *data, GLenum usage);

class ShaderRegistry
{
public:
	static ShaderRegistry &GetInstance()
	{
		static ShaderRegistry instance;
		return instance;
	}

	// Attribute Locs Every Shader Loaded Through LoadShader()
	static const GLuint kPositionAttribLocation = 0;
	static const GLuint kTexCoordAttribLocation = 1;

	bool Initialize()
	{
		if (m_initialized)
			return true;

		s_glCreateShader = (PFNGLCREATESHADERPROC)SDL_GL_GetProcAddress("glCreateShader");
		s_glShaderSource = (PFNGLSHADERSOURCEPROC)SDL_GL_GetProcAddress("glShaderSource");
		s_glCompileShader = (PFNGLCOMPILESHADERPROC)SDL_GL_GetProcAddress("glCompileShader");
		s_glCreateProgram = (PFNGLCREATEPROGRAMPROC)SDL_GL_GetProcAddress("glCreateProgram");
		s_glAttachShader = (PFNGLATTACHSHADERPROC)SDL_GL_GetProcAddress("glAttachShader");
		s_glLinkProgram = (PFNGLLINKPROGRAMPROC)SDL_GL_GetProcAddress("glLinkProgram");
		s_glUseProgram = (PFNGLUSEPROGRAMPROC)SDL_GL_GetProcAddress("glUseProgram");
		s_glGetShaderiv = (PFNGLGETSHADERIVPROC)SDL_GL_GetProcAddress("glGetShaderiv");
		s_glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)SDL_GL_GetProcAddress("glGetShaderInfoLog");
		s_glBindAttribLocation = (PFNGLBINDATTRIBLOCATIONPROC)SDL_GL_GetProcAddress("glBindAttribLocation");
		s_glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)SDL_GL_GetProcAddress("glEnableVertexAttribArray");
		s_glDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC)SDL_GL_GetProcAddress("glDisableVertexAttribArray");
		s_glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)SDL_GL_GetProcAddress("glVertexAttribPointer");
		s_glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)SDL_GL_GetProcAddress("glGetUniformLocation");
		s_glUniform1i = (PFNGLUNIFORM1IPROC)SDL_GL_GetProcAddress("glUniform1i");
		s_glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)SDL_GL_GetProcAddress("glUniformMatrix4fv");
		s_glGenBuffers = (PFNGLGENBUFFERSPROC)SDL_GL_GetProcAddress("glGenBuffers");
		s_glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)SDL_GL_GetProcAddress("glDeleteBuffers");
		s_glBindBuffer = (PFNGLBINDBUFFERPROC)SDL_GL_GetProcAddress("glBindBuffer");
		s_glBufferData = (PFNGLBUFFERDATAPROC)SDL_GL_GetProcAddress("glBufferData");

		if (!s_glCreateShader || !s_glCreateProgram || !s_glUseProgram ||
			!s_glBindAttribLocation || !s_glEnableVertexAttribArray ||
			!s_glDisableVertexAttribArray || !s_glVertexAttribPointer ||
			!s_glGetUniformLocation || !s_glUniform1i || !s_glUniformMatrix4fv ||
			!s_glGenBuffers || !s_glDeleteBuffers || !s_glBindBuffer || !s_glBufferData)
		{
			std::cout << "Shaders Not Supported With GL Configuration\n";
			return false;
		}

		m_initialized = true;

		LoadShader("default", Shaders::Default::vert, Shaders::Default::frag);
		return true;
	}

	bool LoadShader(const std::string &name, const char *vert_source, const char *frag_source)
	{
		if (!m_initialized && !Initialize())
			return false;

		GLuint vs = s_glCreateShader(0x8B31); // GL_VERTEX_SHADER Constant
		s_glShaderSource(vs, 1, &vert_source, NULL);
		s_glCompileShader(vs);

		GLuint fs = s_glCreateShader(0x8B30); // GL_FRAGMENT_SHADER Constant
		s_glShaderSource(fs, 1, &frag_source, NULL);
		s_glCompileShader(fs);

		GLuint program = s_glCreateProgram();
		s_glAttachShader(program, vs);
		s_glAttachShader(program, fs);
		s_glBindAttribLocation(program, kPositionAttribLocation, "a_position");
		s_glBindAttribLocation(program, kTexCoordAttribLocation, "a_texcoord");
		s_glLinkProgram(program);

		m_shaders[name] = program;

		std::cout << "Shader: '" << name.c_str() << "' Compiled And Linked\n";

		return true;
	}

	GLuint GetShader(const std::string &name)
	{
		std::map<std::string, GLuint>::iterator it = m_shaders.find(name);
		if (it != m_shaders.end())
			return it->second;

		return 0; // Fail To Find
	}

	void Use(const std::string &name)
	{
		m_active_program = GetShader(name);
		if (s_glUseProgram)
			s_glUseProgram(m_active_program);
	}

	void Unuse()
	{
		if (s_glUseProgram)
			s_glUseProgram(0);
		m_active_program = 0;
	}

	GLuint CreateVertexBuffer(const vertex_t *vertices, size_t count)
	{
		GLuint vbo = 0;
		s_glGenBuffers(1, &vbo);
		s_glBindBuffer(0x8892 /* GL_ARRAY_BUFFER */, vbo);
		s_glBufferData(0x8892 /* GL_ARRAY_BUFFER */, (GLsizeiptr)(count * sizeof(vertex_t)), vertices, 0x88E4 /* GL_STATIC_DRAW */);
		s_glBindBuffer(0x8892 /* GL_ARRAY_BUFFER */, 0);
		return vbo;
	}

	void DestroyVertexBuffer(GLuint vbo)
	{
		if (s_glDeleteBuffers)
			s_glDeleteBuffers(1, &vbo);
	}

	// Binds vbo And Points a_position/a_texcoord At The Interleaved Vertex Layout
	void BindVertexBuffer(GLuint vbo)
	{
		s_glBindBuffer(0x8892 /* GL_ARRAY_BUFFER */, vbo);

		s_glEnableVertexAttribArray(kPositionAttribLocation);
		s_glVertexAttribPointer(kPositionAttribLocation, 3, GL_FLOAT, GL_FALSE,
								sizeof(vertex_t), (const void *)offsetof(vertex_t, x));

		s_glEnableVertexAttribArray(kTexCoordAttribLocation);
		s_glVertexAttribPointer(kTexCoordAttribLocation, 2, GL_FLOAT, GL_FALSE,
								sizeof(vertex_t), (const void *)offsetof(vertex_t, u));
	}

	void UnbindVertexBuffer()
	{
		s_glDisableVertexAttribArray(kPositionAttribLocation);
		s_glDisableVertexAttribArray(kTexCoordAttribLocation);
		s_glBindBuffer(0x8892 /* GL_ARRAY_BUFFER */, 0);
	}

	void SetMatrix(const std::string &name, const GLfloat *matrix4x4)
	{
		GLint loc = s_glGetUniformLocation(m_active_program, name.c_str());
		if (loc >= 0)
			s_glUniformMatrix4fv(loc, 1, GL_FALSE, matrix4x4);
	}

	// Binds `texture` To Texture Unit 0 And Points The Shader's Sampler Uniform At It
	void SetTexture(const std::string &name, GLuint texture)
	{
		glBindTexture(GL_TEXTURE_2D, texture);
		GLint loc = s_glGetUniformLocation(m_active_program, name.c_str());
		if (loc >= 0)
			s_glUniform1i(loc, 0);
	}

private:
	ShaderRegistry() : m_initialized(false), m_active_program(0) {}
	~ShaderRegistry() {}

	std::map<std::string, GLuint> m_shaders;
	bool m_initialized;
	GLuint m_active_program;

	static PFNGLCREATESHADERPROC s_glCreateShader;
	static PFNGLSHADERSOURCEPROC s_glShaderSource;
	static PFNGLCOMPILESHADERPROC s_glCompileShader;
	static PFNGLCREATEPROGRAMPROC s_glCreateProgram;
	static PFNGLATTACHSHADERPROC s_glAttachShader;
	static PFNGLLINKPROGRAMPROC s_glLinkProgram;
	static PFNGLUSEPROGRAMPROC s_glUseProgram;
	static PFNGLGETSHADERIVPROC s_glGetShaderiv;
	static PFNGLGETSHADERINFOLOGPROC s_glGetShaderInfoLog;
	static PFNGLBINDATTRIBLOCATIONPROC s_glBindAttribLocation;
	static PFNGLENABLEVERTEXATTRIBARRAYPROC s_glEnableVertexAttribArray;
	static PFNGLDISABLEVERTEXATTRIBARRAYPROC s_glDisableVertexAttribArray;
	static PFNGLVERTEXATTRIBPOINTERPROC s_glVertexAttribPointer;
	static PFNGLGETUNIFORMLOCATIONPROC s_glGetUniformLocation;
	static PFNGLUNIFORM1IPROC s_glUniform1i;
	static PFNGLUNIFORMMATRIX4FVPROC s_glUniformMatrix4fv;
	static PFNGLGENBUFFERSPROC s_glGenBuffers;
	static PFNGLDELETEBUFFERSPROC s_glDeleteBuffers;
	static PFNGLBINDBUFFERPROC s_glBindBuffer;
	static PFNGLBUFFERDATAPROC s_glBufferData;
};

PFNGLCREATESHADERPROC ShaderRegistry::s_glCreateShader = NULL;
PFNGLSHADERSOURCEPROC ShaderRegistry::s_glShaderSource = NULL;
PFNGLCOMPILESHADERPROC ShaderRegistry::s_glCompileShader = NULL;
PFNGLCREATEPROGRAMPROC ShaderRegistry::s_glCreateProgram = NULL;
PFNGLATTACHSHADERPROC ShaderRegistry::s_glAttachShader = NULL;
PFNGLLINKPROGRAMPROC ShaderRegistry::s_glLinkProgram = NULL;
PFNGLUSEPROGRAMPROC ShaderRegistry::s_glUseProgram = NULL;
PFNGLGETSHADERIVPROC ShaderRegistry::s_glGetShaderiv = NULL;
PFNGLGETSHADERINFOLOGPROC ShaderRegistry::s_glGetShaderInfoLog = NULL;
PFNGLBINDATTRIBLOCATIONPROC ShaderRegistry::s_glBindAttribLocation = NULL;
PFNGLENABLEVERTEXATTRIBARRAYPROC ShaderRegistry::s_glEnableVertexAttribArray = NULL;
PFNGLDISABLEVERTEXATTRIBARRAYPROC ShaderRegistry::s_glDisableVertexAttribArray = NULL;
PFNGLVERTEXATTRIBPOINTERPROC ShaderRegistry::s_glVertexAttribPointer = NULL;
PFNGLGETUNIFORMLOCATIONPROC ShaderRegistry::s_glGetUniformLocation = NULL;
PFNGLUNIFORM1IPROC ShaderRegistry::s_glUniform1i = NULL;
PFNGLUNIFORMMATRIX4FVPROC ShaderRegistry::s_glUniformMatrix4fv = NULL;
PFNGLGENBUFFERSPROC ShaderRegistry::s_glGenBuffers = NULL;
PFNGLDELETEBUFFERSPROC ShaderRegistry::s_glDeleteBuffers = NULL;
PFNGLBINDBUFFERPROC ShaderRegistry::s_glBindBuffer = NULL;
PFNGLBUFFERDATAPROC ShaderRegistry::s_glBufferData = NULL;

#endif /* SHADER_REGISTRY_H */