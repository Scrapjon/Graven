#ifndef ENEMYBIRD_H
#define ENEMYBIRD_H

#include "Entities/AI/FlyingAIEntity.h"

class EnemyBird : public FlyingAIEntity
{
    EnemyBird() : FlyingAIEntity(ET_ENEMY_BIRD) {}
    virtual ~EnemyBird() {}
};

#endif /* ENEMYBIRD_H */