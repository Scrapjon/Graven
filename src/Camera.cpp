#include "Camera.h"

void Camera::SetPosition(float x, float y, float z)
{
	m_position[0] = x;
	m_position[1] = y;
	m_position[2] = z;
	m_dirty = true;
}

void Camera::GetPosition(float &x, float &y, float &z) const
{
	x = m_position[0];
	y = m_position[1];
	z = m_position[2];
}

void Camera::SetYawPitch(float yaw_degrees, float pitch_degrees)
{
	m_yaw = yaw_degrees;
	m_pitch = ClampPitch(pitch_degrees);
	RebuildVectors();
	m_dirty = true;
}

void Camera::AddYawPitch(float yaw_delta, float pitch_delta)
{
	SetYawPitch(m_yaw + yaw_delta, m_pitch + pitch_delta);
}

void Camera::SetPerspective(float fovy_degrees, float aspect, float znear, float zfar)
{
	m_fovy_degrees = fovy_degrees;
	m_aspect = aspect;
	m_znear = znear;
	m_zfar = zfar;
	m_dirty = true;
}

void Camera::SetAspectRatio(float aspect)
{
	m_aspect = aspect;
	m_dirty = true;
}

void Camera::MoveForward(float amount)
{
	m_position[0] += m_forward[0] * amount;
	m_position[1] += m_forward[1] * amount;
	m_position[2] += m_forward[2] * amount;
	m_dirty = true;
}

void Camera::MoveRight(float amount)
{
	m_position[0] += m_right[0] * amount;
	m_position[1] += m_right[1] * amount;
	m_position[2] += m_right[2] * amount;
	m_dirty = true;
}

void Camera::MoveUp(float amount)
{
	m_position[1] += amount;
	m_dirty = true;
}

const float *Camera::GetViewMatrix()
{
	if (m_dirty)
		RebuildMatrices();
	return m_view;
}

const float *Camera::GetProjectionMatrix()
{
	if (m_dirty)
		RebuildMatrices();
	return m_projection;
}

const float *Camera::GetViewProjectionMatrix()
{
	if (m_dirty)
		RebuildMatrices();
	return m_view_projection;
}

float Camera::ClampPitch(float pitch_degrees)
{
	if (pitch_degrees > 89.f)
		return 89.f;
	if (pitch_degrees < -89.f)
		return -89.f;
	return pitch_degrees;
}

void Camera::Normalize(float v[3])
{
	float len = std::sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	if (len > 0.00001f)
	{
		v[0] /= len;
		v[1] /= len;
		v[2] /= len;
	}
}

void Camera::Cross(const float a[3], const float b[3], float out[3])
{
	out[0] = a[1] * b[2] - a[2] * b[1];
	out[1] = a[2] * b[0] - a[0] * b[2];
	out[2] = a[0] * b[1] - a[1] * b[0];
}

float Camera::Dot(const float a[3], const float b[3])
{
	return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

void Camera::MatrixMultiply(const float *a, const float *b, float *out)
{
	float result[16];
	for (int col = 0; col < 4; ++col)
	{
		for (int row = 0; row < 4; ++row)
		{
			result[col * 4 + row] =
				a[0 * 4 + row] * b[col * 4 + 0] +
				a[1 * 4 + row] * b[col * 4 + 1] +
				a[2 * 4 + row] * b[col * 4 + 2] +
				a[3 * 4 + row] * b[col * 4 + 3];
		}
	}
	for (int i = 0; i < 16; ++i)
		out[i] = result[i];
}

void Camera::RebuildVectors()
{
	float yaw = DegToRad(m_yaw);
	float pitch = DegToRad(m_pitch);

	m_forward[0] = std::cos(pitch) * std::cos(yaw);
	m_forward[1] = std::sin(pitch);
	m_forward[2] = std::cos(pitch) * std::sin(yaw);
	Normalize(m_forward);

	const float world_up[3] = {0.f, 1.f, 0.f};
	Cross(m_forward, world_up, m_right);
	Normalize(m_right);

	Cross(m_right, m_forward, m_up);
	Normalize(m_up);
}

void Camera::RebuildMatrices()
{
	RebuildVectors();

	float center[3] =
		{
			m_position[0] + m_forward[0],
			m_position[1] + m_forward[1],
			m_position[2] + m_forward[2],
		};

	const float &s0 = m_right[0], &s1 = m_right[1], &s2 = m_right[2];
	const float &u0 = m_up[0], &u1 = m_up[1], &u2 = m_up[2];
	const float &f0 = m_forward[0], &f1 = m_forward[1], &f2 = m_forward[2];
	(void)center;

	m_view[0] = s0;
	m_view[4] = s1;
	m_view[8] = s2;
	m_view[12] = -Dot(m_right, m_position);
	m_view[1] = u0;
	m_view[5] = u1;
	m_view[9] = u2;
	m_view[13] = -Dot(m_up, m_position);
	m_view[2] = -f0;
	m_view[6] = -f1;
	m_view[10] = -f2;
	m_view[14] = Dot(m_forward, m_position);
	m_view[3] = 0.f;
	m_view[7] = 0.f;
	m_view[11] = 0.f;
	m_view[15] = 1.f;

	for (int i = 0; i < 16; ++i)
		m_projection[i] = 0.f;

	float f = 1.f / std::tan(DegToRad(m_fovy_degrees) * 0.5f);
	m_projection[0] = f / m_aspect;
	m_projection[5] = f;
	m_projection[10] = (m_zfar + m_znear) / (m_znear - m_zfar);
	m_projection[11] = -1.f;
	m_projection[14] = (2.f * m_zfar * m_znear) / (m_znear - m_zfar);

	MatrixMultiply(m_projection, m_view, m_view_projection);

	m_dirty = false;
}
