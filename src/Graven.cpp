#include "Graven.h"

#include "Registry/ShaderRegistry.h"
#include "Registry/TextureRegistry.h"
#include "Camera.h"
#include "World.h"

#include "Types/GUID.h"
#include "Types/Vector.h"
#include "Types/Mesh.h"
#include "Entities/Player.h"

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

	screen = SDL_SetVideoMode(g_start_width, g_start_height, 32, SDL_OPENGL | SDL_HWSURFACE);

	if (screen == NULL)
	{
		std::cout << "Failed To Create SDL Window\n";
		running = false;
		SDL_Quit();
		return 1;
	}

	std::cout << "Loading Shader Registry...\n";

	if (!ShaderRegistry::GetInstance().Initialize())
		std::cout << "ShaderRegistry Initialization Failed Critically!\n";

	std::cout << "Loading Texture Registry...\n";

	if (!TextureRegistry::GetInstance().Initialize())
		std::cout << "TextureRegistry Initialization Failed Critically!\n";

	SDL_WM_SetCaption("Graven", "Graven");

	glViewport(0, 0, g_start_width, g_start_height);

	glEnable(GL_DEPTH_TEST);

	vertex_t triangle[6] =
		{
			{-.5f, 0.5f, 0.f, /**/ 0.0f, 1.0f},
			{0.5f, -.5f, 0.f, /**/ 1.0f, 0.0f},
			{-.5f, -.5f, 0.f, /**/ 0.0f, 0.0f},
			//
			{-.5f, 0.5f, 0.f, /**/ 0.0f, 1.0f},
			{0.5f, 0.5f, 0.f, /**/ 1.0f, 1.0f},
			{0.5f, -.5f, 0.f, /**/ 1.0f, 0.0f},
		};

	ShaderRegistry &shaders = ShaderRegistry::GetInstance();
	GLuint triangle_vbo = shaders.CreateVertexBuffer(triangle, 6);
	GLuint checker_texture = TextureRegistry::GetInstance().GetTexture("test");

	// Camera camera;
	// World::SetActiveCamera(&camera);

	// World::GetActiveCamera()->SetPosition(96.f, 1000.f, 0.f);
	// World::GetActiveCamera()->AddYawPitch(-20.f, 0.f);
	// World::GetActiveCamera()->SetPerspective(45.f, (GLfloat)g_start_width / (GLfloat)g_start_height, 0.1f, 10000.f);

	World::LoadMap("./GravenHub.gmap");

	Player *player = new Player();

	World::Spawn(player);

	player->SetPos(World::GetPlayerStart());

	SDL_Event event;

	mesh_t mesh;
	mesh.LoadShader("default");

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// World::GetActiveCamera()->SetPosition(World::GetPlayerStart());

	World::BeginPlay();

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

		World::Tick(1.f / 60.f);

		glClearColor(.1f, .1f, .1f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shaders.Use("default");
		shaders.SetMatrix("u_mvp", World::GetActiveCamera()->GetViewProjectionMatrix());
		shaders.SetTexture("u_texture", checker_texture);

		// shaders.BindVertexBuffer(triangle_vbo);
		// glDrawArrays(GL_TRIANGLES, 0, 6);
		// shaders.UnbindVertexBuffer();

		World::Draw();

		mesh.Draw();

		shaders.Unuse();

		SDL_GL_SwapBuffers();
	}

	delete player;
	player = 0;

	shaders.DestroyVertexBuffer(triangle_vbo);
	glDeleteTextures(1, &checker_texture);

	SDL_Quit();

	return 0;
}