#ifndef VECTOR_H
#define VECTOR_H

typedef struct vec3_t
{
	float x, y, z;

	vec3_t() : x(0.f), y(0.f), z(0.f) {}

	vec3_t(float flat) : x(flat), y(flat), z(flat) {}

	vec3_t(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}

	// Operators
	vec3_t operator+(const vec3_t &other) const
	{
		return vec3_t(x + other.x, y + other.y, z + other.z);
	}
	vec3_t &operator+=(const vec3_t &rhs)
	{
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;
		return *this;
	}
	vec3_t operator-(const vec3_t &other) const
	{
		return vec3_t(x - other.x, y - other.y, z - other.z);
	}
	vec3_t &operator-=(const vec3_t &rhs)
	{
		x -= rhs.x;
		y -= rhs.y;
		z -= rhs.z;
		return *this;
	}
	vec3_t operator*(const vec3_t &other) const
	{
		return vec3_t(x * other.x, y * other.y, z * other.z);
	}
	vec3_t &operator*=(const vec3_t &rhs)
	{
		x *= rhs.x;
		y *= rhs.y;
		z *= rhs.z;
		return *this;
	}
	vec3_t operator/(const vec3_t &other) const
	{
		return vec3_t(x / other.x, y / other.y, z / other.z);
	}
	vec3_t &operator/=(const vec3_t &rhs)
	{
		x /= rhs.x;
		y /= rhs.y;
		z /= rhs.z;
		return *this;
	}

	vec3_t operator-() const
	{
		return vec3_t(-x, -y, -z);
	}

	float operator|(const vec3_t &other) const
	{
		return Dot(*this, other);
	}

	vec3_t operator^(const vec3_t &other) const
	{
		return Cross(*this, other);
	}

	// Math
	static float Dot(const vec3_t &lhs, const vec3_t &rhs)
	{
		return (lhs.x * rhs.x) + (lhs.y * rhs.y) + (lhs.z * rhs.z);
	}

	static vec3_t Cross(const vec3_t &lhs, const vec3_t &rhs)
	{
		return vec3_t(
			lhs.y * rhs.z - lhs.z * rhs.y,
			lhs.z * rhs.x - lhs.x * rhs.z,
			lhs.x * rhs.y - lhs.y * rhs.x);
	}

	// Global Constants
	static vec3_t X()
	{
		static const vec3_t x = vec3_t(1.f, 0.f, 0.f);
		return x;
	}

	static vec3_t Y()
	{
		static const vec3_t y = vec3_t(0.f, 1.f, 0.f);
		return y;
	}

	static vec3_t Z()
	{
		static const vec3_t z = vec3_t(0.f, 0.f, 1.f);
		return z;
	}

} vec3_t;

#endif /* VECTOR_H */