#ifndef OBJECT_H
#define OBJECT_H

#include "Types/Vector.h"
#include "Types/Bounds.h"
#include "Types/GUID.h"

class Object
{
	guid_t m_guid;
	bool m_tick_enabled;

public:
	const char *name;

	Object()
		: m_guid(guid_t::GetNewGUID()), m_tick_enabled(true), name("Entity"),
		  _pos(), _rot(), _scale(vec3_t(1.f)), _size(vec3_t(32.f)) {};
	virtual ~Object() {};

	guid_t GetGuid() const { return m_guid; }
	bool GetTickEnabled() const { return m_tick_enabled; }
	void SetTickEnabled(bool enabled) { m_tick_enabled = enabled; }
	vec3_t GetPos() const { return _pos; }
	void SetPos(const vec3_t pos) { _pos = pos; }
	vec3_t GetRot() const { return _rot; }
	void SetRot(vec3_t rot) { _rot = rot; }
	vec3_t GetScale() { return _scale; }
	void SetScale(vec3_t scale) { _scale = scale; }
	bounds_t GetBounds() const { return bounds_t(_pos - (_size * 0.5f), _pos + (_size * 0.5f)); }

	virtual void BeginPlay() = 0;
	virtual void Tick(float delta_time) = 0;
	virtual void EndPlay() = 0;

	virtual void Draw() = 0;

	void Destroy();

protected:
	vec3_t _pos;
	vec3_t _rot;
	vec3_t _scale;
	vec3_t _size;
};

#endif /* OBJECT_H */