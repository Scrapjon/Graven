#ifndef TRACE_H
#define TRACE_H

#include "Types/Vector.h"

typedef struct
{
	bool all_solid : 1;	  // If True, Plane Is Not Valid
	bool start_solid : 1; // If True, Trace Was Start Penetrating
	bool in_open : 1;	  // Trace Ended In Air
	bool in_water : 1;	  // Trace Position Is Underwater
	float fraction;		  // Percent Of The Trace Dist Hit (1.f Didn't Hit Anything)
	vec3_t end_pos;		  // Hit Pos
	vec3_t normal;		  // Hit Normal

	// Entity *entity;	  // Hit Entity

} trace_t;

#endif /* TRACE_H */