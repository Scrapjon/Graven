#ifndef ENEMYWIZARD_H
#define ENEMYWIZARD_H

#include "Entities/AI/GroundAIEntity.h"

class EnemyWizard : GroundAIEntity
{
public:
    EnemyWizard() : GroundAIEntity(ET_ENEMY_WIZARD) {}
    virtual ~EnemyWizard() {}

    virtual void Think(float) {};
    virtual void BeginPlay() {};
    virtual void EndPlay() {};
    virtual void Draw() {};
    virtual void HandleDeath(Object *, damage_type_t) {};
};

#endif /* ENEMYWIZARD_H */