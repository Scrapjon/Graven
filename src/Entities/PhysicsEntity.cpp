#include "PhysicsEntity.h"
#include "World.h"

namespace
{
	// How Many Times To Test Rehits
	const int kMaxSlideIterations = 4;

	// The Collision Pushback
	const float kSkinWidth = 0.03125f;

	// Walkable Surface In Rads ~40 deg
	const float kGroundNormalY = 0.7f;

	const float kGroundFriction = 2.f;
	const float kAirResistance = 1.f;

	const float kStepHeight = 32.f;
}

vec3_t PhysicsEntity::ClipVelocity(const vec3_t &in, const vec3_t &normal, float overbounce)
{
	const float backoff = vec3_t::Dot(in, normal) * overbounce;
	return in - (normal * backoff);
}

void PhysicsEntity::Tick(float delta_time)
{
	if (_use_gravity)
		_vel += vec3_t(0.f, World::GetGravity(), 0.f) * delta_time;

	_vel += _accel * delta_time;

	// Apply Friction / Drag
	if (_grounded)
	{
		float damping = 1.0f - (kGroundFriction * delta_time);
		if (damping < 0.f)
			damping = 0.f;

		_vel.x *= damping;
		_vel.z *= damping;
	}
	else
	{
		float air_damping = 1.0f - (kAirResistance * delta_time);
		if (air_damping < 0.f)
			air_damping = 0.f;

		_vel.x *= air_damping;
		_vel.z *= air_damping;
	}

	const vec3_t half_size = _size * 0.5f;
	const vec3_t mins = -half_size;
	const vec3_t maxs = half_size;

	vec3_t pos = _pos;
	vec3_t remaining_move = _vel * delta_time;

	vec3_t original_vel = _vel;

	const bool was_grounded = _grounded;
	_grounded = false;

	for (int iter = 0; iter < kMaxSlideIterations; ++iter)
	{
		if (vec3_t::Length(remaining_move) < 0.0001f)
			break;

		const vec3_t target = pos + remaining_move;
		const trace_t trace = World::Trace(pos, target, mins, maxs);

		pos = trace.end_pos;

		if (trace.fraction >= 1.f)
			break;

		if (was_grounded && trace.normal.y <= kGroundNormalY)
		{
			const vec3_t step_start = pos + vec3_t(0.f, kStepHeight, 0.f);
			const vec3_t step_end = target + vec3_t(0.f, -kStepHeight, 0.f);
			const trace_t step_trace = World::Trace(step_start, step_end, mins, maxs);

			if (step_trace.fraction > trace.fraction && (step_trace.fraction >= 1.f || step_trace.normal.y > kGroundNormalY))
			{
				pos = step_trace.end_pos + vec3_t(0.f, kSkinWidth, 0.f);
				_grounded = true;

				_vel = original_vel;

				if (_vel.y < 0.f)
					_vel.y = 0.f;

				float fraction_used = trace.fraction;
				remaining_move = (_vel * delta_time) * (1.f - fraction_used);

				continue;
			}
		}

		pos += trace.normal * kSkinWidth;

		if (trace.normal.y > kGroundNormalY)
			_grounded = true;

		const float remaining_fraction = 1.f - trace.fraction;
		remaining_move = ClipVelocity(remaining_move * remaining_fraction, trace.normal, 1.f);
		_vel = ClipVelocity(_vel, trace.normal, 1.f);
	}

	_pos = pos;

	if (_grounded && _vel.y < 0.f)
		_vel.y = 0.f;

	_accel = vec3_t(0.f);
}