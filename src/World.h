#ifndef WORLD_H
#define WORLD_H

#include "Camera.h"
#include "Object.h"
#include "Types/Bounds.h"
#include "Types/Brush.h"
#include "Types/Trace.h"
#include "Types/TriggerVolume.h"
#include <map>
#include <string>
#include <vector>
#include <iostream>

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

	// Load ".gmap" Level
	static bool LoadMap(const std::string &path);

	// Box Sweep Trace
	static trace_t Trace(const vec3_t &start, const vec3_t &end, const vec3_t &mins, const vec3_t &maxs);

	static float GetGravity() { return Get().m_gravity; }
	static vec3_t GetPlayerStart() { return Get().m_player_start; }

	static vec3_t GetBrushPos()
	{
		return Get().m_brushes[0]->GetBounds().Center();
	}

private:
	static unsigned int s_timer;
	static bool s_has_started;

	Camera *m_camera;
	float m_gravity;
	float m_ambient_strength;
	vec3_t m_ambient_color;
	vec3_t m_player_start;

	std::vector<Object *> m_objects;
	std::vector<Object *> m_pending_destroys;
	std::vector<brush_t *> m_brushes;

	std::vector<trigger_volume_t> m_triggers;
	// info_teleport_destination Positions, Keyed By "targetname"
	std::map<std::string, vec3_t> m_teleport_destinations;

	static World &Get()
	{
		static World instance;
		return instance;
	}

	void Cleanup();
	bool LoadCompiledMap(const std::string &filepath);

	// Teleports Object Overlapping A trigger_teleport Volume To info_teleport_destination
	static void CheckTriggers();

	World();
	~World();

	World(const World &);
	World &operator=(const World &);
};

#endif