#ifndef ENTITY_H
#define ENTITY_H

#include "Object.h"
#include "Types/Mesh.h"

typedef enum
{

} entity_type_t;

typedef enum
{
    EFLAG_NONE = 0,
    EFLAG_GODMODE = 1 << 0, // No Damage
    EFLAG_GHOST = 1 << 1,   // Invisible To AI And Volume Triggers
    EFLAG_NOCLIP = 1 << 2,  // Ignore Collision And Fly Movement
    EFLAG_FROZEN = 1 << 3,  // Do Not Update Or Think (AI)
    EFLAG_WRATH = 1 << 4    // Any Damage Dealt By Entity Is Set To uint Max
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
    entity_type_t _entity_type;
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