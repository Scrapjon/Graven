#ifndef GRAVEN_H
#define GRAVEN_H

#include <SDL/SDL.h>
#include <GL/gl.h>

class Graven
{
public:
	static Graven &Get()
	{
		static Graven instance;
		return instance;
	}

	int Run();

private:
	Graven()
		: screen(NULL) {};
	~Graven() {};
	Graven(const Graven &);
	Graven &operator=(const Graven &);

	SDL_Surface *screen;
};

#endif /* GRAVEN_H */