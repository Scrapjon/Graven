#ifndef WORLD_H
#define WORLD_H

#include "Camera.h"

#define MAX_WORLD_OBJECTS 256

class World
{
public:
	static Camera *GetActiveCamera()
	{
		return GetActiveWorld().m_camera;
	}

	static void SetActiveCamera(Camera *cam)
	{
		GetActiveWorld().m_camera = cam;
	}

	static void BeginPlay() {}
	static void Tick(float /* delta_time */)
	{
		s_timer++;
	}
	static void EndPlay() {}

private:
	static unsigned int s_timer;

	static World &GetActiveWorld()
	{
		static World instance;
		return instance;
	}

	World() : m_camera(0), m_gravity(-9.8f) {}
	~World() {}

	Camera *m_camera;

	float m_gravity;
};

#endif /* WORLD_H */