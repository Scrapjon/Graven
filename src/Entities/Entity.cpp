#include "Entities/Entity.h"
#include "Entity.h"

bool Entity::Damage(Object *causer, damage_type_t reason, unsigned int amount)
{
	const bool is_dead = _health < amount;
	_health = is_dead ? 0 : _health - amount;
	return is_dead;
}

bool Entity::AddFlag(flag_entity_t flag)
{
	const bool already_has_flag = (_flags & flag) > 0;
	_flags = _flags | flag;
	return already_has_flag;
}

bool Entity::RemoveFlag(flag_entity_t flag)
{
	const bool removed = (_flags & flag) > 0;
	_flags = _flags | ~flag;
	return removed;
}
