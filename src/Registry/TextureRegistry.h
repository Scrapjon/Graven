#ifndef TEXTURE_REGISTRY_H
#define TEXTURE_REGISTRY_H

#include <SDL/SDL.h>
#include <GL/gl.h>
#include <string>
#include <map>
#include <iostream>

typedef unsigned int GLenum;
typedef unsigned int GLuint;
typedef int GLsizei;
typedef int GLint;
typedef void(APIENTRY *PFNGLGENTEXTURESPROCV)(GLsizei n, GLuint *textures);
typedef void(APIENTRY *PFNGLBINDTEXTUREPROC)(GLenum target, GLuint texture);
typedef void(APIENTRY *PFNGLTEXIMAGE2DPROC)(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels);
typedef void(APIENTRY *PFNGLTEXPARAMETERIPROC)(GLenum target, GLenum pname, GLint param);
typedef void(APIENTRY *PFNGLDELETETEXTURESPROCV)(GLsizei n, const GLuint *textures);
typedef void(APIENTRY *PFNGLACTIVETEXTUREPROC)(GLenum texture);

class TextureRegistry
{
public:
	static TextureRegistry &GetInstance()
	{
		static TextureRegistry instance;
		return instance;
	}

	bool Initialize();

	GLuint GetTexture(const std::string &name)
	{
		std::map<std::string, GLuint>::iterator it = m_textures.find(name);
		if (it != m_textures.end())
			return it->second;

		return 0; // Fail To Find
	}

	// Activates The Given Texture To The unit
	void Bind(const std::string &name, GLenum unit)
	{
		GLuint texture = GetTexture(name);
		s_glActiveTexture(unit);
		s_glBindTexture(GL_TEXTURE_2D, texture);
	}

	void Unbind(GLenum target)
	{
		s_glBindTexture(target, 0);
	}

private:
	TextureRegistry() : m_extensions_loaded(false) {}
	~TextureRegistry() {}

	// File Loading
	// !! Will Need To Load From Baked Eventually !!
	bool LoadTexture(const std::string &name, const std::string &filepath)
	{
		if (!m_extensions_loaded && !Initialize())
			return false;

		SDL_Surface *raw = SDL_LoadBMP(filepath.c_str());
		if (!raw)
		{
			std::cout << "LoadTexture Failed To Load '" << filepath.c_str() << "'\n";
			return false;
		}

		SDL_Surface *rgba = SDL_CreateRGBSurface(SDL_SWSURFACE, raw->w, raw->h, 32, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);

		SDL_SetAlpha(raw, 0, 0);
		SDL_BlitSurface(raw, NULL, rgba, NULL);
		SDL_FreeSurface(raw);

		int width = rgba->w;
		int height = rgba->h;
		int pitch = width * 4;
		unsigned char *flipped = new unsigned char[pitch * height];
		const unsigned char *src = (const unsigned char *)rgba->pixels;

		for (int y = 0; y < height; ++y)
		{
			// Bottom Up Copy Row
			memcpy(flipped + y * pitch, src + (height - 1 - y) * pitch, pitch);
		}

		GLuint texture = 0;
		s_glGenTextures(1, &texture);
		s_glBindTexture(GL_TEXTURE_2D, texture);
		s_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		s_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		s_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		s_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		s_glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, flipped);
		s_glBindTexture(GL_TEXTURE_2D, 0);

		delete[] flipped;
		SDL_FreeSurface(rgba);

		m_textures[name] = texture;

		std::cout << "Texture '" << name.c_str() << "' Loaded From '" << filepath.c_str() << "' Successfully\n";

		return true;
	}

	// Generates A Small Checkerboard Pattern
	void CreateCheckerTexture(const std::string &name)
	{
		const int size = 8;
		unsigned char pixels[size * size * 4];

		for (int y = 0; y < size; ++y)
		{
			for (int x = 0; x < size; ++x)
			{
				bool light = ((x + y) % 2) == 0;
				unsigned char value = light ? 220 : 40;
				int i = (y * size + x) * 4;
				pixels[i + 0] = value;
				pixels[i + 1] = value;
				pixels[i + 2] = value;
				pixels[i + 3] = 255;
			}
		}

		GLuint texture = 0;
		s_glGenTextures(1, &texture);
		s_glBindTexture(GL_TEXTURE_2D, texture);
		s_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		s_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		s_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		s_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		s_glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size, size, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
		s_glBindTexture(GL_TEXTURE_2D, 0);

		m_textures[name] = texture;

		std::cout << "Texture: '" << name.c_str() << "' Generated\n";
	}

	std::map<std::string, GLuint> m_textures;
	bool m_extensions_loaded;

	static PFNGLGENTEXTURESPROCV s_glGenTextures;
	static PFNGLBINDTEXTUREPROC s_glBindTexture;
	static PFNGLTEXIMAGE2DPROC s_glTexImage2D;
	static PFNGLTEXPARAMETERIPROC s_glTexParameteri;
	static PFNGLDELETETEXTURESPROCV s_glDeleteTextures;
	static PFNGLACTIVETEXTUREPROC s_glActiveTexture;
};

#endif /* TEXTURE_REGISTRY_H */