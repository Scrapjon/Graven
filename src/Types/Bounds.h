#ifndef BOUNDS_H
#define BOUNDS_H

#include <algorithm>

#include "Vector.h"

typedef struct bounds_t
{
	vec3_t min;
	vec3_t max;

	bounds_t() : min(0.f), max(0.f) {}
	bounds_t(const vec3_t &min_, const vec3_t &max_) : min(min_), max(max_) {}

	vec3_t Center() const
	{
		return (min + max) * .5f;
	}
	vec3_t Extents() const
	{
		return (max - min) * .5f;
	}

	void EncapsulatePoint(const vec3_t &point)
	{
		min.x = std::min(min.x, point.x);
		min.y = std::min(min.y, point.y);
		min.z = std::min(min.z, point.z);

		max.x = std::max(max.x, point.x);
		max.y = std::max(max.y, point.y);
		max.z = std::max(max.z, point.z);
	}

} bounds_t;

#endif /* BOUNDS_H */