#ifndef TRIGGER_VOLUME_H
#define TRIGGER_VOLUME_H

#include "Types/Bounds.h"
#include <string>

typedef enum
{
	TT_TELE, // trigger_teleport
	TT_CMD,	 // trigger_command
} trigger_type_t;

typedef struct
{
	trigger_type_t type : 7;
	bool triggered : 1;

	bounds_t bounds;
	std::string target;
} trigger_volume_t;

#endif /* TRIGGER_VOLUME_H */