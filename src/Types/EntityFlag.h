#ifndef ENTITY_FLAG_H
#define ENTITY_FLAG_H

typedef enum
{
	EFLAG_NONE = 0,
	EFLAG_GODMODE = 1 << 0, // No Damage
	EFLAG_GHOST = 1 << 1,	// Invisible To AI And Volume Triggers
	EFLAG_NOCLIP = 1 << 2,	// Ignore Collision And Fly Movement
	EFLAG_FROZEN = 1 << 3,	// Do Not Update Or Think (AI)
	EFLAG_WRATH = 1 << 4,	// Any Damage Dealt By Entity Is Set To uint Max
} entity_flag_t;

#endif /* ENTITY_FLAG_H */