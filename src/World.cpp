#include "World.h"
#include "Registry/ShaderRegistry.h"
#include "Entities/PhysicsEntity.h"
#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>

#define MAX_WORLD_OBJECTS 256

unsigned int World::s_timer = 0;
bool World::s_has_started = false;

namespace
{
	template <typename T>
	bool ReadRaw(std::istream &in, T &value)
	{
		in.read(reinterpret_cast<char *>(&value), sizeof(T));
		return !in.fail();
	}

	bool ReadString(std::istream &in, std::string &s)
	{
		int len = 0;
		if (!ReadRaw(in, len) || len < 0)
			return false;

		s.resize((size_t)len);
		if (len > 0)
			in.read(&s[0], len);

		return !in.fail();
	}
} // namespace

World::World() : m_camera(0), m_gravity(-980.f), m_player_start(0.f), m_ambient_strength(0.25f), m_ambient_color(1.0f, 1.0f, 1.0f)
{
	m_objects.reserve(MAX_WORLD_OBJECTS);
	m_pending_destroys.reserve(MAX_WORLD_OBJECTS / 8);
}

World::~World()
{
	Cleanup();

	for (size_t i = 0; i < m_objects.size(); ++i)
		delete m_objects[i];

	for (size_t i = 0; i < m_brushes.size(); ++i)
		delete m_brushes[i];
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

	ShaderRegistry &shaders = ShaderRegistry::GetInstance();
	shaders.SetVec3("ambientColor", world.m_ambient_color);
	shaders.SetFloat("ambientStrength", world.m_ambient_strength);

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

	CheckTriggers();

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

	for (size_t i = 0; i < world.m_brushes.size(); ++i)
	{
		if (world.m_brushes[i])
			world.m_brushes[i]->Draw();
	}

	if (!world.m_pending_destroys.empty())
		world.Cleanup();
}

void World::CheckTriggers()
{
	World &world = Get();

	if (world.m_triggers.empty())
		return;

	for (size_t t = 0; t < world.m_triggers.size(); ++t)
	{
		trigger_volume_t &trigger = world.m_triggers[t];

		for (size_t i = 0; i < world.m_objects.size(); ++i)
		{
			Object *obj = world.m_objects[i];
			if (!obj)
				continue;

			const bounds_t obj_bounds = obj->GetBounds();

			// Object And Volume Are AABB
			const bool overlaps =
				obj_bounds.max.x >= trigger.bounds.min.x && obj_bounds.min.x <= trigger.bounds.max.x &&
				obj_bounds.max.y >= trigger.bounds.min.y && obj_bounds.min.y <= trigger.bounds.max.y &&
				obj_bounds.max.z >= trigger.bounds.min.z && obj_bounds.min.z <= trigger.bounds.max.z;

			if (!overlaps)
			{
				if (trigger.triggered)
					trigger.triggered = false;

				continue;
			}

			if (trigger.triggered)
				continue;

			switch (trigger.type)
			{
			case TT_TELE:
			{

				std::map<std::string, vec3_t>::const_iterator dest = world.m_teleport_destinations.find(trigger.target);
				if (dest == world.m_teleport_destinations.end())
					continue;

				obj->SetPos(dest->second);
				break;
			}
			case TT_CMD:
			{
				std::cout << "NOT IMPLEMENTED\n";
				break;
			}

			default:
				break;
			}

			trigger.triggered = true;
		}
	}
}

trace_t World::Trace(const vec3_t &start, const vec3_t &end, const vec3_t &mins, const vec3_t &maxs)
{
	World &world = Get();

	trace_t trace;
	trace.end_pos = end;

	for (size_t i = 0; i < world.m_brushes.size(); ++i)
	{
		if (world.m_brushes[i])
			world.m_brushes[i]->TraceBox(start, end, mins, maxs, trace);
	}

	return trace;
}

bool World::LoadMap(const std::string &path)
{
	std::string filepath = path;

	return Get().LoadCompiledMap(filepath);
}

bool World::LoadCompiledMap(const std::string &filepath)
{
	std::ifstream file(filepath.c_str(), std::ios::binary);
	if (!file.is_open())
	{
		std::cout << "LoadMap: Failed To Open '" << filepath.c_str() << "'\n";
		return false;
	}

	char magic[4] = {0};
	file.read(magic, 4);
	if (file.fail() || std::memcmp(magic, "GMPB", 4) != 0)
	{
		std::cout << "LoadMap: '" << filepath.c_str() << "' Is Not A Valid GMAP File\n";
		return false;
	}

	for (size_t i = 0; i < m_brushes.size(); ++i)
		delete m_brushes[i];
	m_brushes.clear();
	m_player_start = vec3_t(0.f);
	m_triggers.clear();
	m_teleport_destinations.clear();

	int brush_count = 0;
	ReadRaw(file, brush_count);
	std::cout << "Loading " << brush_count << " Brushes\n";
	m_brushes.reserve(brush_count);

	for (int b = 0; b < brush_count; ++b)
	{
		brush_t *brush = new brush_t();

		int face_count = 0;
		ReadRaw(file, face_count);

		for (int f = 0; f < face_count; ++f)
		{
			brush_face_t face;
			ReadString(file, face.texture);

			int vert_count = 0;
			ReadRaw(file, vert_count);
			face.verts.resize(vert_count);

			if (vert_count > 0)
				file.read(reinterpret_cast<char *>(face.verts.data()), vert_count * sizeof(vertex_t));

			brush->AddFace(face);
		}

		brush->BuildRenderData();
		m_brushes.push_back(brush);
	}

	int entity_count = 0;
	ReadRaw(file, entity_count);

	for (int e = 0; e < entity_count; ++e)
	{
		std::string classname;
		ReadString(file, classname);

		int kv_count = 0;
		ReadRaw(file, kv_count);

		vec3_t origin(0.f);
		bool has_origin = false;
		std::string targetname;

		for (int k = 0; k < kv_count; ++k)
		{
			std::string key, value;
			ReadString(file, key);
			ReadString(file, value);

			if (key == "origin")
			{
				std::istringstream iss(value);
				iss >> origin.x >> origin.y >> origin.z;
				has_origin = true;
			}
			else if (key == "targetname")
			{
				targetname = value;
			}
		}

		if (classname == "player_start" && has_origin)
			m_player_start = origin + vec3_t(0.f, 1.f, 0.f);
		else if (classname == "info_teleport_destination" && has_origin && !targetname.empty())
			m_teleport_destinations[targetname] = origin + vec3_t(0.f, 33.f, 0.f);
	}

	// Volume/Trigger Entities
	int volume_entity_count = 0;
	ReadRaw(file, volume_entity_count);

	for (int e = 0; e < volume_entity_count; ++e)
	{
		std::string classname;
		ReadString(file, classname);

		int kv_count = 0;
		ReadRaw(file, kv_count);

		std::string target;

		for (int k = 0; k < kv_count; ++k)
		{
			std::string key, value;
			ReadString(file, key);
			ReadString(file, value);

			if (key == "target")
				target = value;
		}

		vec3_t bounds_min(0.f), bounds_max(0.f);
		ReadRaw(file, bounds_min);
		ReadRaw(file, bounds_max);

		if (!target.empty())
		{
			trigger_volume_t trigger;

			if (classname == "trigger_teleport")
				trigger.type = TT_TELE;
			else if (classname == "trigger_command")
				trigger.type = TT_CMD;

			trigger.bounds = bounds_t(bounds_min, bounds_max);
			trigger.target = target;
			m_triggers.push_back(trigger);
		}
	}

	// TODO: Add in logic to grab ambient light values from map file.

	std::cout << "LoadMap: Loaded '" << filepath.c_str() << "' (" << m_brushes.size() << " Brushes, " << m_triggers.size() << " Triggers)\n";

	return true;
}