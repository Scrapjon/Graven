#ifndef VERTEX_H
#define VERTEX_H

#include "Vector.h"

typedef struct
{
	float x, y, z; // WS Pos
	float u, v;	   // UV

	operator vec3_t() const
	{
		return vec3_t(x, y, z);
	}

} vertex_t;

#endif /* VERTEX_H */