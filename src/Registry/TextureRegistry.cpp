#include "TextureRegistry.h"

PFNGLGENTEXTURESPROCV TextureRegistry::s_glGenTextures = NULL;
PFNGLBINDTEXTUREPROC TextureRegistry::s_glBindTexture = NULL;
PFNGLTEXIMAGE2DPROC TextureRegistry::s_glTexImage2D = NULL;
PFNGLTEXPARAMETERIPROC TextureRegistry::s_glTexParameteri = NULL;
PFNGLDELETETEXTURESPROCV TextureRegistry::s_glDeleteTextures = NULL;
PFNGLACTIVETEXTUREPROC TextureRegistry::s_glActiveTexture = NULL;

bool TextureRegistry::Initialize()
{
	if (m_extensions_loaded)
		return true;

	s_glGenTextures = (PFNGLGENTEXTURESPROCV)SDL_GL_GetProcAddress("glGenTextures");
	s_glBindTexture = (PFNGLBINDTEXTUREPROC)SDL_GL_GetProcAddress("glBindTexture");
	s_glTexImage2D = (PFNGLTEXIMAGE2DPROC)SDL_GL_GetProcAddress("glTexImage2D");
	s_glTexParameteri = (PFNGLTEXPARAMETERIPROC)SDL_GL_GetProcAddress("glTexParameteri");
	s_glDeleteTextures = (PFNGLDELETETEXTURESPROCV)SDL_GL_GetProcAddress("glDeleteTextures");
	s_glActiveTexture = (PFNGLACTIVETEXTUREPROC)SDL_GL_GetProcAddress("glActiveTexture");

	if (!s_glGenTextures || !s_glBindTexture || !s_glTexImage2D ||
		!s_glTexParameteri || !s_glDeleteTextures || !s_glActiveTexture)
	{
		std::cout << "Textures Not Supported With GL Configuration\n";
		return false;
	}

	m_extensions_loaded = true;

	CreateCheckerTexture("checker");

	LoadTexture("test", "./test.bmp");

	// Transparent

	// !! TODO: Have Animated Sprites Using World Timer !!

	return true;
}
