#include "Object.h"

void Object::Destroy()
{
	m_pending_kill = true;
}

bool Object::IsValid() const
{
	return m_pending_kill;
}
