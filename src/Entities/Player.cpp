#include "Player.h"

#include "World.h"

#include <SDL/SDL.h>

namespace
{
	const float g_move_speed = 400.f;
	const float g_jump_speed = 250.f;
} // namespace

Player::Player()
	: PhysicsEntity(ET_PLAYER), m_has_magic(false), m_mana(0), m_camera(new Camera())
{
	_size = vec3_t(32.f, 64.f, 32.f);
}

Player::~Player()
{
	delete m_camera;
	m_camera = 0;
}

void Player::BeginPlay()
{
	World::SetActiveCamera(m_camera);

	World::GetActiveCamera()->SetPosition(96.f, 1000.f, 0.f);
	World::GetActiveCamera()->AddYawPitch(-20.f, 0.f);
	World::GetActiveCamera()->SetPerspective(60.f, (GLfloat)800 / (GLfloat)600, 0.1f, 10000.f);
}

void Player::Tick(float delta_time)
{
	HandleInput();
	PhysicsEntity::Tick(delta_time);
	m_camera->SetPosition(_pos + vec3_t(0.f, 32.f, 0.f));
}

void Player::HandleInput()
{
	Uint8 *keystate = SDL_GetKeyState(NULL);
	if (!keystate)
		return;

	vec3_t move_dir(0.f, 0.f, 0.f);

	float forward_input = 0.f;
	float right_input = 0.f;

	if (keystate[SDLK_w] || keystate[SDLK_UP])
	{
		forward_input += 1.f;
	}
	if (keystate[SDLK_s] || keystate[SDLK_DOWN])
	{
		forward_input -= 1.f;
	}
	if (keystate[SDLK_a] || keystate[SDLK_LEFT])
	{
		right_input -= 1.f;
	}
	if (keystate[SDLK_d] || keystate[SDLK_RIGHT])
	{
		right_input += 1.f;
	}

	if (keystate[SDLK_KP8])
	{
		m_camera->AddYawPitch(0.f, 2.f);
	}
	if (keystate[SDLK_KP5])
	{
		m_camera->AddYawPitch(0.f, -2.f);
	}
	if (keystate[SDLK_KP4])
	{
		m_camera->AddYawPitch(-3.f, 0.f);
	}
	if (keystate[SDLK_KP6])
	{
		m_camera->AddYawPitch(3.f, 0.f);
	}

	if (forward_input != 0.f || right_input != 0.f)
	{
		// !! TODO: Move DegToRad Func To A Common.h !!
		float yaw_rad = m_camera->GetYaw() * 3.14159265f / 180.f;

		vec3_t cam_forward = vec3_t::Normalize(m_camera->GetForward() * vec3_t(1, 0, 1));
		vec3_t cam_right = vec3_t::Normalize(m_camera->GetRight() * vec3_t(1, 0, 1));

		move_dir = (cam_forward * forward_input) + (cam_right * right_input);

		float len = vec3_t::Length(move_dir);
		if (len > 0.0f)
		{
			move_dir.x /= len;
			move_dir.z /= len;
		}

		_accel = move_dir * g_move_speed;
	}
	else
	{
		_accel = vec3_t(0.f, 0.f, 0.f);
	}

	if (keystate[SDLK_SPACE] && _grounded)
	{
		_vel.y = g_jump_speed;
		_grounded = false;
	}
}

void Player::Draw()
{
}

void Player::HandleDeath(Object *causer, damage_type_t reason)
{
}