#include "Object.h"

#include "World.h"

void Object::Destroy()
{
	World::Destroy(this);
}