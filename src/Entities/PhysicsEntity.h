#ifndef PHYSICSENTITY_H
#define PHYSICSENTITY_H

#include "Entities/Entity.h"
#include "Types/Faction.h"

class PhysicsEntity : public Entity
{
protected:
	vec3_t _vel;
	vec3_t _accel;
	float _mass;

	PhysicsEntity(entity_type_t entity_type) : Entity(entity_type) {}

	virtual ~PhysicsEntity() {};

public:
	faction_t faction;

	virtual void BeginPlay() = 0;
	virtual void Tick(float delta_time) = 0; // TODO: Implement handling of physics
	virtual void EndPlay() = 0;

	const vec3_t &GetVelocity() { return _vel; }
	void SetVelocity(const vec3_t &vel) { _vel = vel; }

	const vec3_t &GetAcceleration() { return _accel; };
	void SetAcceleration(const vec3_t &accel) { _accel = accel; }

	float GetMass() { return _mass; }
	void SetMass(float mass) { _mass = mass; }
};

#endif /* PHYSICSENTITY_H */