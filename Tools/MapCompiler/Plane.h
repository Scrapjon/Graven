#ifndef PLANE_H
#define PLANE_H

#include "Types/Vector.h"

typedef struct plane_t
{
	vec3_t normal;
	float dist;

	plane_t() : normal(0.f, 0.f, 1.f), dist(0.f) {}
	plane_t(const vec3_t &normal_, float dist_) : normal(normal_), dist(dist_) {}

	static plane_t FromPoints(const vec3_t &p0, const vec3_t &p1, const vec3_t &p2)
	{
		const vec3_t normal = vec3_t::Normalize(vec3_t::Cross(p2 - p0, p1 - p0));
		return plane_t(normal, vec3_t::Dot(normal, p0));
	}

	float DistanceTo(const vec3_t &point) const
	{
		return vec3_t::Dot(normal, point) - dist;
	}

} plane_t;

#endif /* PLANE_H */