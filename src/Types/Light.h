#ifndef LIGHT_H
#define LIGHT_H

#include "Object.h"

class Light : public Object
{
    Light(float intensity = 1.0f, vec3_t light_color = vec3_t(1.f, 1.f, 1.f));
    vec3_t light_color;
    float intensity;

    virtual void Draw();
};
#endif /* LIGHT_H */
