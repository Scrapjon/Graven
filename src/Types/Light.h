#ifndef LIGHT_H
#define LIGHT_H

#include "Object.h"

class Light : public Object
{
    Light(vec3_t light_color = vec3_t(1.f, 1.f, 1.f));
    vec3_t light_color;
};
#endif /* LIGHT_H */
