#include "Graven.h"

#include <iostream>

int Graven::Run()
{
	bool running = true;

	std::cout << "Graven Launching...\n";

	std::cout << "Creating SDL Window...\n";

	if (SDL_Init(SDL_INIT_EVERYTHING) == -1)
	{
		std::cout << "Failed To Initialize SDL\n";
		running = false;
	}

	screen = SDL_SetVideoMode(800, 600, 32, SDL_HWSURFACE);

	if (screen == NULL)
	{
		std::cout << "Failed To Create SDL Window\n";
		running = false;
	}

	SDL_Event event;

	while (running)
	{
		// Callbacks
		SDL_PollEvent(&event);

		if (event.type == SDL_QUIT)
		{
			running = false;
		}

		// Draw
		SDL_FillRect(screen, NULL, 600);
		SDL_Flip(screen);
	}

	SDL_Quit();

	return 0;
}