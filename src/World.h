#ifndef WORLD_H
#define WORLD_H

#include "Camera.h"
#include "Object.h"
#include <vector>

class World
{
public:
	static Camera *GetActiveCamera() { return Get().m_camera; }
	static void SetActiveCamera(Camera *cam) { Get().m_camera = cam; }

	static void BeginPlay();
	static void Tick(float delta_time);
	static void EndPlay();
	static void Draw();

	static unsigned int GetTimer() { return s_timer; }

	static bool Spawn(Object *obj);
	static void Destroy(Object *obj);

private:
	struct ObjectEntry
	{
		Object *obj;
		bool pending_destroy;
	};

	static unsigned int s_timer;
	static bool s_has_started;

	Camera *m_camera;
	float m_gravity;
	std::vector<ObjectEntry> m_objects;

	static World &Get()
	{
		static World instance;
		return instance;
	}

	void Cleanup();

	World();
	~World();

	World(const World &);
	World &operator=(const World &);
};

#endif