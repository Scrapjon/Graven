#ifndef ENTITY_H
#define ENTITY_H

#include "Object.h"
#include "Types/Mesh.h"

typedef enum
{

} flag_entity_t;

typedef enum
{
    DT_MELEE = 0,
    DT_RANGED,
    DT_FIRE,
    DT_EXPLOSIVE
} damage_type_t;

class Entity : Object
{
protected:
    unsigned short _health;
    unsigned short _flags;
    mesh_t _mesh;

    Entity() {}
    ~Entity() {}

public:
    virtual void BeginPlay() = 0;
    virtual void Tick(float delta_time) = 0;
    virtual void EndPlay() = 0;

    virtual void Draw() = 0;

    bool Damage(Object *causer, damage_type_t reason, unsigned int amount);
    virtual void HandleDeath(Object *causer, damage_type_t reason) = 0;

    bool AddFlag(flag_entity_t flag);
    bool RemoveFlag(flag_entity_t flag);
};

#endif /* ENTITY_H */