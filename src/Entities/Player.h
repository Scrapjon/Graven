#ifndef PLAYER_H
#define PLAYER_H

#include "Entities/PhysicsEntity.h"
#include "Camera.h"

class Player : public PhysicsEntity
{
private:
	bool m_has_magic : 1;	  // Whether The Player Has Magic Unlocked
	unsigned char m_mana : 7; // The Mana [ 0, 127 ]

public:
	Player();
	~Player();

	virtual void BeginPlay();
	virtual void Tick(float delta_time);
	virtual void EndPlay() {}
	void HandleInput();

	virtual void Draw();
	virtual void HandleDeath(Object *causer, damage_type_t reason);

private:
	Camera *m_camera;
};

#endif /* PLAYER_H */