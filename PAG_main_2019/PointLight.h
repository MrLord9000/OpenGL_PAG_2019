#pragma once

#include "Light.h"
#include <sstream>

class PointLight : public Light
{
public:
	static int pointLightCount;

	PointLight(glm::vec3 position, glm::vec3 color, float intensity, float radius)
		: position(position), color(color), intensity(intensity), radius(radius),
		Light(pointLightCount, "Models/primitives/sphere.obj")
	{
		pointLightCount++;
		sceneLights.push_back(this);
	}

	void UpdateLight()
	{
		for (auto e : Light::litShaders)
		{
			std::ostringstream name;
			
			name << "pointLights[" << GetNumber() << "].position";
			e->SetVec3(name.str(), position);
			name.clear();

			name << "pointLights[" << GetNumber() << "].color";
			e->SetVec3(name.str(), color);
			name.clear();

			name << "pointLights[" << GetNumber() << "].intensity";
			e->SetFloat(name.str(), intensity);
			name.clear();

			name << "pointLights[" << GetNumber() << "].radius";
			e->SetFloat(name.str(), radius);
			name.clear();
		}
	}

private:
	glm::vec3 position;
	glm::vec3 color;
	float intensity, radius;
};