#include "Types/Light.h"
#include "Registry/ShaderRegistry.h"

Light::Light(float intensity, vec3_t light_color) : intensity(intensity), light_color(light_color), Object()
{
}
void Light::Draw()
{
    ShaderRegistry &shaders = ShaderRegistry::GetInstance();

    shaders.SetFloat("intensity", intensity);
    shaders.SetVec3("lightColor", light_color);
}