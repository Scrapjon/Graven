#include "ShaderRegistry.h"

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