#include "World.h"
#include <algorithm>

#define MAX_WORLD_OBJECTS 256

unsigned int World::s_timer = 0;
bool World::s_has_started = false;

World::World() : m_camera(0), m_gravity(-9.8f)
{
	m_objects.reserve(MAX_WORLD_OBJECTS);
	m_pending_destroys.reserve(MAX_WORLD_OBJECTS / 8);
}

World::~World()
{
	Cleanup();
	for (size_t i = 0; i < m_objects.size(); ++i)
		delete m_objects[i];
}

bool World::Spawn(Object *obj)
{
	if (!obj)
		return false;

	World &world = Get();
	if (world.m_objects.size() >= MAX_WORLD_OBJECTS)
		return false;

	world.m_objects.push_back(obj);

	if (s_has_started)
		obj->BeginPlay();

	return true;
}

void World::Destroy(Object *obj)
{
	if (!obj)
		return;
	Get().m_pending_destroys.push_back(obj);
}

void World::Cleanup()
{
	World &world = Get();
	if (world.m_pending_destroys.empty())
		return;

	for (size_t i = 0; i < world.m_pending_destroys.size(); ++i)
	{
		Object *obj = world.m_pending_destroys[i];
		if (!obj)
			continue;

		if (s_has_started)
			obj->EndPlay();

		std::vector<Object *>::iterator it = std::remove(world.m_objects.begin(), world.m_objects.end(), obj);
		world.m_objects.erase(it, world.m_objects.end());

		delete obj;
	}
	world.m_pending_destroys.clear();
}

void World::BeginPlay()
{
	s_has_started = true;
	World &world = Get();
	for (size_t i = 0; i < world.m_objects.size(); ++i)
	{
		if (world.m_objects[i])
			world.m_objects[i]->BeginPlay();
	}
}

void World::Tick(float delta_time)
{
	s_timer++;
	World &world = Get();

	for (size_t i = 0; i < world.m_objects.size(); ++i)
	{
		Object *obj = world.m_objects[i];
		if (obj && obj->GetTickEnabled())
			obj->Tick(delta_time);
	}

	if (!world.m_pending_destroys.empty())
		world.Cleanup();
}

void World::EndPlay()
{
	s_has_started = false;
	World &world = Get();

	for (size_t i = 0; i < world.m_objects.size(); ++i)
	{
		if (world.m_objects[i])
			world.m_objects[i]->EndPlay();
	}

	world.Cleanup();
}

void World::Draw()
{
	World &world = Get();
	for (size_t i = 0; i < world.m_objects.size(); ++i)
	{
		if (world.m_objects[i])
			world.m_objects[i]->Draw();
	}

	if (!world.m_pending_destroys.empty())
		world.Cleanup();
}