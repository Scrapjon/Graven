#ifndef ENEMYBIRD_H
#define ENEMYBIRD_H

#include "Entities/AI/FlyingAIEntity.h"

class EnemyBird : public FlyingAIEntity
{
    EnemyBird() : FlyingAIEntity(ET_ENEMY_BIRD) {}
    virtual ~EnemyBird() {}

    virtual void Think(float) {};
    virtual void BeginPlay() {};
    virtual void EndPlay() {};
    virtual void Draw() { FlyingAIEntity::Draw(); }
    virtual void HandleDeath(Object *, damage_type_t) {};
};

#endif /* ENEMYBIRD_H */