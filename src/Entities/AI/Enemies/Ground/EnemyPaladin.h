#ifndef ENEMYPALADIN_H
#define ENEMYPALADIN_H

#include "Entities/AI/GroundAIEntity.h"

class EnemyPaladin : GroundAIEntity
{
public:
	EnemyPaladin() : GroundAIEntity(ET_ENEMY_PALADIN) {}
	virtual ~EnemyPaladin() {}

	virtual void Think(float) {};
	virtual void BeginPlay() {};
	virtual void EndPlay() {};
	virtual void Draw() { GroundAIEntity::Draw(); };
	virtual void HandleDeath(Object *, damage_type_t) {};
};

#endif /* ENEMYPALADIN_H */