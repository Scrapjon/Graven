#ifndef ENEMYDECAYED_H
#define ENEMYDECAYED_H

#include "Entities/AI/GroundAIEntity.h"

class EnemyDecayed : public GroundAIEntity
{
public:
    EnemyDecayed() : GroundAIEntity(ET_ENEMY_DECAYED) {}
    virtual ~EnemyDecayed() {}

    virtual void Think(float) {};
    virtual void BeginPlay() {};
    virtual void EndPlay() {};
    virtual void Draw() { GroundAIEntity::Draw(); };
    virtual void HandleDeath(Object *, damage_type_t) {};
};

#endif /* ENEMYDECAYED_H */