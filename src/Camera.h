#ifndef CAMERA_H
#define CAMERA_H

#include <cmath>

class Camera
{
public:
	Camera()
		: m_yaw(-90.f), m_pitch(0.f),
		  m_fovy_degrees(45.f),
		  m_aspect(4.f / 3.f),
		  m_znear(0.1f), m_zfar(100.f),
		  m_dirty(true)
	{
		m_position[0] = m_position[1] = m_position[2] = 0.f;
		RebuildVectors();
	}

	void SetPosition(float x, float y, float z);

	void GetPosition(float &x, float &y, float &z) const;

	void SetYawPitch(float yaw_degrees, float pitch_degrees);

	void AddYawPitch(float yaw_delta, float pitch_delta);

	void SetPerspective(float fovy_degrees, float aspect, float znear, float zfar);

	// Call When The Window Is Resized So The Projection Matrix Stays Correct
	void SetAspectRatio(float aspect);

	// Moves Along The Camera's Forward Vector
	void MoveForward(float amount);

	// Moves Along The Camera's Right Vector
	void MoveRight(float amount);

	// Moves Along World Up (0, 1, 0)
	void MoveUp(float amount);

	const float *GetViewMatrix();

	const float *GetProjectionMatrix();

	const float *GetViewProjectionMatrix();

private:
	static float ClampPitch(float pitch_degrees);

	static float DegToRad(float degrees)
	{
		return degrees * 3.14159265f / 180.f;
	}

	static void Normalize(float v[3]);

	static void Cross(const float a[3], const float b[3], float out[3]);

	static float Dot(const float a[3], const float b[3]);

	static void MatrixMultiply(const float *a, const float *b, float *out);

	// Recomputes The Forward/Right/Up Basis Vectors From Yaw/Pitch
	void RebuildVectors();

	// Right Handed Look At View Matrix
	void RebuildMatrices();

	float m_position[3];
	float m_yaw;
	float m_pitch;

	float m_forward[3];
	float m_right[3];
	float m_up[3];

	float m_fovy_degrees;
	float m_aspect;
	float m_znear;
	float m_zfar;

	float m_view[16];
	float m_projection[16];
	float m_view_projection[16];
	bool m_dirty;
};

#endif /* CAMERA_H */