#pragma once

#include "Material.h"
#include "Model.h"

#include <glm/glm/glm.hpp>
#include <list>

class Light
{
public:
	static std::list<Shader*> litShaders;
	static std::list<Light*> sceneLights;
	
	Light(int number, const char* gizmoPath, Shader* gizmoShader = nullptr)
		: number(number)
	{
		if (gizmoShader == nullptr)
		{
			gizmoMaterial = new Material(new Shader("Shaders/unlitColor.vert", "Shaders/unlitColor.frag"));
			defaultShader = true;
		}
		else
		{
			gizmoMaterial = new Material(gizmoShader);
			defaultShader = false;
		}
		gizmoModel = new Model(gizmoPath);
		dirty = true;
	}

	~Light()
	{
		if (defaultShader)
		{
			gizmoMaterial->DestroyShader();
		}
		delete gizmoModel;
		delete gizmoMaterial;
	}

	static void LightPass()
	{
		for (auto e : sceneLights)
		{
			if (e->dirty)
			{
				e->UpdateLight();
				e->dirty = false;
			}
			e->gizmoModel->Draw(e->gizmoMaterial);
		}
	}

	virtual void UpdateLight() = 0;
	int GetNumber() { return number; }

protected:
	bool dirty;
	Model* gizmoModel;
	Material* gizmoMaterial;

private:
	int number;
	bool defaultShader;
};