#ifndef ENEMYDARRELL_H
#define ENEMYDARRELL_H

#include "Entities/AI/GroundAIEntity.h"

class EnemyDarrell : GroundAIEntity
{
public:
    EnemyDarrell() : GroundAIEntity(ET_ENEMY_DARRELL) {}
    virtual ~EnemyDarrell() {}

    virtual void Think(float) {};
    virtual void BeginPlay() {};
    virtual void EndPlay() {};
    virtual void Draw() {};
    virtual void HandleDeath(Object *, damage_type_t) {};
};

#endif /* ENEMYDARRELL_H */