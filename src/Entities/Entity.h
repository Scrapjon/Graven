#ifndef ENTITY_H
#define ENTITY_H

#include <cassert>
#include "Object.h"
#include "Types/Mesh.h"

typedef enum
{
    ET_ENTITY = 0, // Should never be this
    ET_PLAYER,
    ET_AI,
    ET_ENEMY_BIRD,
    ET_ENEMY_DECAYED,
    ET_ENEMY_PALADIN,
    ET_ENEMY_WIZARD,
    ET_ENEMY_DARRELL

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

class Entity : public Object
{

private:
    const entity_type_t m_entity_type;

protected:
    unsigned short _health;
    unsigned short _flags;

    mesh_t _mesh;

    Entity(entity_type_t entity_type) : m_entity_type(entity_type)
    {
        assert(entity_type != ET_ENTITY && "Entity initialized as Entity Type (ET_ENTITY)");
    }
    virtual ~Entity() {}

public:
    virtual void BeginPlay() = 0;
    virtual void Tick(float delta_time) = 0;
    virtual void EndPlay() = 0;

    virtual void Draw() = 0;

    bool Damage(Object *causer, damage_type_t reason, unsigned int amount);
    virtual void HandleDeath(Object *causer, damage_type_t reason) = 0;

    bool AddFlag(flag_entity_t flag);
    bool RemoveFlag(flag_entity_t flag);

    entity_type_t GetEntityType() const { return m_entity_type; }                                 // Returns entity type
    unsigned short GetEntityTypeId() const { return static_cast<unsigned short>(m_entity_type); } // Returns numerical representation of entity type
};

#endif /* ENTITY_H */