#ifndef GROUNDAIENTITY_H
#define GROUNDAIENTITY_H

#include "Entities/AI/AIEntity.h"

class GroundAIEntity : public AIEntity
{
protected:
    GroundAIEntity(entity_type_t entity_type) : AIEntity(entity_type) {}
    virtual ~GroundAIEntity() {};
};

#endif /* GROUNDAIENTITY_H */