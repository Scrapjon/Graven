#include "AIEntity.h"
#include "World.h"

void AIEntity::Tick(float delta_time)
{
    /* Uncomment when GetTimer is implemented
    if ((World::GetTimer() + m_offset) % 2)
        Think(delta_time);
    */
}

bool AIEntity::MoveStep(const vec3_t &move)
{
    return false;
}

trace_t AIEntity::Move(const vec3_t &start, const vec3_t &mins, const vec3_t &maxs, const vec3_t &end)
{
    return trace_t();
}

trace_t AIEntity::Attack(damage_type_t type, unsigned short amount, float dist)
{
    return trace_t();
}
