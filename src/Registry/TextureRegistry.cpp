#include "TextureRegistry.h"

PFNGLGENTEXTURESPROCV TextureRegistry::s_glGenTextures = NULL;
PFNGLBINDTEXTUREPROC TextureRegistry::s_glBindTexture = NULL;
PFNGLTEXIMAGE2DPROC TextureRegistry::s_glTexImage2D = NULL;
PFNGLTEXPARAMETERIPROC TextureRegistry::s_glTexParameteri = NULL;
PFNGLDELETETEXTURESPROCV TextureRegistry::s_glDeleteTextures = NULL;
PFNGLACTIVETEXTUREPROC TextureRegistry::s_glActiveTexture = NULL;