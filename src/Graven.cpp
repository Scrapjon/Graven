#include "Graven.h"

#include <iostream>

static int g_start_width = 800;
static int g_start_height = 600;

int Graven::Run()
{
	bool running = true;

	std::cout << "Graven Launching...\n";

	std::cout << "Creating SDL Window...\n";

	if (SDL_Init(SDL_INIT_EVERYTHING) == -1)
	{
		std::cout << "Failed To Initialize SDL\n";
		running = false;
		return 1;
	}

	// 32 Bits Total
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	screen = SDL_SetVideoMode(g_start_width, g_start_height, 8, SDL_OPENGL | SDL_HWSURFACE);

	if (screen == NULL)
	{
		std::cout << "Failed To Create SDL Window\n";
		running = false;
		SDL_Quit();
		return 1;
	}

	SDL_WM_SetCaption("Graven", "Graven");

	glViewport(0, 0, g_start_width, g_start_height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.f, g_start_width, g_start_height, 0.f, -1.f, 1.f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	SDL_Event event;

	while (running)
	{
		// Handle Events
		while (SDL_PollEvent(&event))
		{

			if (event.type == SDL_QUIT)
			{
				running = false;
			}
		}

		glClearColor(.1f, .1f, .1f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBegin(GL_TRIANGLES);

		glColor3f(1.f, 0.f, 0.f);
		glVertex2f(400.f, 150.f);

		glColor3f(0.f, 1.f, 0.f);
		glVertex2f(600.f, 450.f);

		glColor3f(0.f, 0.f, 1.f);
		glVertex2f(200.f, 450.f);

		glEnd();

		SDL_GL_SwapBuffers();
	}

	SDL_Quit();

	return 0;
}