#ifndef AIENTITY_H
#define AIENTITY_H

#include "Entities/PhysicsEntity.h"

#define MAX_AI_OFFSET 3

typedef struct
{
    bool all_solid;   // If True, Plane Is Not Valid
    bool start_solid; // If True, Trace Was Start Penetrating
    bool in_open;
    bool in_water;
    float fraction; // Percent Of The Trace Dist Hit (1.f Didn't Hit Anything)
    vec3_t end_pos; // Hit Pos
    vec3_t normal;  // Hit Normal
    Entity *entity; // Hit Entity
} trace_t;

class AIEntity : public PhysicsEntity
{
private:
    unsigned char m_offset; // Offset to prevent all AI ticks from being at the same time.

protected:
    AIEntity(entity_type_t entity_type) : PhysicsEntity(entity_type), m_offset(rand() % MAX_AI_OFFSET) {}
    ~AIEntity() {}

    virtual void Think(float delta_time) = 0;

    virtual void Tick(float delta_time);

    // Attempt To Move Entity To Position
    bool MoveStep(const vec3_t &move);

    // Raycast The Move And Return The Trace
    trace_t Move(const vec3_t &start, const vec3_t &mins, const vec3_t &maxs, const vec3_t &end);

    // Raycast For Entities And Apply Damage
    trace_t Attack(damage_type_t type, unsigned short amount, float dist);
};

#endif /* AIENTITY_H */