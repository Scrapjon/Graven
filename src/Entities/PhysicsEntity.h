#ifndef PHYSICSENTITY_H
#define PHYSICSENTITY_H

#include "Entities/Entity.h"
#include "Types/Faction.h"
#include "Types/Trace.h"

class PhysicsEntity : public Entity
{
protected:
	vec3_t _vel;
	vec3_t _accel;
	float _mass;

	bool _use_gravity;
	bool _grounded;

	PhysicsEntity(entity_type_t entity_type)
		: Entity(entity_type), _vel(), _accel(), _mass(1.f), _use_gravity(true), _grounded(false), faction(FC_BADGUYS) {}

	virtual ~PhysicsEntity() {};

public:
	faction_t faction;

	virtual void BeginPlay() = 0;

	virtual void Tick(float delta_time);

	virtual void EndPlay() = 0;

	const vec3_t &GetVelocity() { return _vel; }
	void SetVelocity(const vec3_t &vel) { _vel = vel; }

	const vec3_t &GetAcceleration() { return _accel; };
	void SetAcceleration(const vec3_t &accel) { _accel = accel; }

	float GetMass() { return _mass; }
	void SetMass(float mass) { _mass = mass; }

	void SetUseGravity(bool use_gravity) { _use_gravity = use_gravity; }

	// True If Entity Was On Geometry Last Tick
	bool IsGrounded() const { return _grounded; }

private:
	static vec3_t ClipVelocity(const vec3_t &in, const vec3_t &normal, float overbounce);
};

#endif /* PHYSICSENTITY_H */