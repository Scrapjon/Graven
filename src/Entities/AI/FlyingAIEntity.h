#ifndef FLYINGAIENTITY_H
#define FLYINGAIENTITY_H

#include "Entities/AI/AIEntity.h"

class FlyingAIEntity : public AIEntity
{
protected:
    FlyingAIEntity(entity_type_t entity_type) : AIEntity(entity_type) {}
    virtual ~FlyingAIEntity() {}
};

#endif /* FLYINGAIENTITY_H */