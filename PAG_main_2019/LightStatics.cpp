#include "Light.h"
#include "PointLight.h"

std::list<Shader*> Light::litShaders = std::list<Shader*>();
std::list<Light*> Light::sceneLights = std::list<Light*>();
int PointLight::pointLightCount = 0;