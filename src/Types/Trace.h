#ifndef TRACE_H
#define TRACE_H

#include "Vector.h"

class Entity;

typedef struct trace_t
{
	bool all_solid : 2;	  // If True, Plane Is Not Valid
	bool start_solid : 2; // If True, Trace Was Start Penetrating
	bool in_open : 2;	  // Trace Ended In Air
	bool in_water : 2;	  // Trace Position Is Underwater
	float fraction;		  // Percent Of The Trace Dist Hit (1.f Didn't Hit Anything)
	vec3_t end_pos;		  // Hit Pos
	vec3_t normal;		  // Hit Normal
	Entity *entity;		  // Hit Entity

	trace_t()
		: all_solid(false), start_solid(false), in_open(true), in_water(false),
		  fraction(1.f), end_pos(0.f), normal(0.f), entity(0)
	{
	}

} trace_t;

#endif /* TRACE_H */