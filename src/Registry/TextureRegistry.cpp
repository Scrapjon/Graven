#include "TextureRegistry.h"

PFNGLGENTEXTURESPROCV TextureRegistry::s_glGenTextures = NULL;
PFNGLBINDTEXTUREPROC TextureRegistry::s_glBindTexture = NULL;
PFNGLTEXIMAGE2DPROC TextureRegistry::s_glTexImage2D = NULL;
PFNGLTEXPARAMETERIPROC TextureRegistry::s_glTexParameteri = NULL;
PFNGLDELETETEXTURESPROCV TextureRegistry::s_glDeleteTextures = NULL;
PFNGLACTIVETEXTUREPROC TextureRegistry::s_glActiveTexture = NULL;

#define RTEX(name) \
	LoadTexture(name, "./" name ".bmp");

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
	RTEX("makkon/makkon_building_textures/win_teal1_1");
	RTEX("makkon/makkon_building_textures/woodc2_cbare01");
	RTEX("makkon/makkon_building_textures/woodc2_wbare01");
	RTEX("makkon/makkon_industrial_textures/_ind_fnc4_blk1");
	RTEX("makkon/makkon_industrial_textures/ind_c05_grey1");
	RTEX("makkon/makkon_industrial_textures/ind_c07_grey1");
	RTEX("makkon/makkon_nature_textures/sand02_red1");
	RTEX("makkon/makkon_stone_textures/stn_c07_tan1");
	RTEX("makkon/makkon_stone_textures/stn_f13_tan1");
	RTEX("makkon/makkon_stone_textures/stn_f15_tan1");
	RTEX("makkon/makkon_stone_textures/stn_f16c_tan1");
	RTEX("makkon/makkon_stone_textures/stn_t01a_tan1");
	RTEX("makkon/makkon_stone_textures/stn_t01b_tan1");
	RTEX("makkon/makkon_stone_textures/stn_t02b_tan1");
	RTEX("makkon/makkon_stone_textures/stn_t05_tan1");
	RTEX("makkon/makkon_stone_textures/stn_t07_tan1");
	RTEX("makkon/makkon_stone_textures/stn_t11_tan1");
	RTEX("makkon/makkon_stone_textures/tan1");
	// !! TODO: Have Animated Sprites Using World Timer !!

	return true;
}
