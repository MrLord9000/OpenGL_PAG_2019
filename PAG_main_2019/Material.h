#pragma once

#include "Shader.h"

#include <map>
#include <string>

class Material
{
public:
	Material(Shader* shader) : shader(shader) {}

	void SetShader(Shader* shader) { this->shader = shader; }
	Shader* GetShader() { return shader; }

	void SetModel(glm::mat4* matrix) { model = matrix; }
	void SetView(glm::mat4* matrix) { view = matrix; }
	void SetProjection(glm::mat4* matrix) { projection = matrix; }
	void SetViewPosition(glm::vec3* viewPos) { viewPosition = viewPos; }

	void SetFloat(float value, const std::string name)
	{
		std::map<std::string, float>::iterator it = floats.find(name);
		if (it != floats.end())
		{
			it->second = value;
		}
		else
		{
			floats.insert({ name, value });
		}
	}

	void SetVec4(glm::vec4 value, const std::string name)
	{
		std::map<std::string, glm::vec4>::iterator it = vector4s.find(name);
		if (it != vector4s.end())
		{
			it->second = value;
		}
		else
		{
			vector4s.insert({ name, value });
		}
	}

	void SetVec3(glm::vec3 value, const std::string name)
	{
		std::map<std::string, glm::vec3>::iterator it = vector3s.find(name);
		if (it != vector3s.end())
		{
			it->second = value;
		}
		else
		{
			vector3s.insert({ name, value });
		}
	}

	void SetInt(int value, const std::string name)
	{
		std::map<std::string, int>::iterator it = ints.find(name);
		if (it != ints.end())
		{
			it->second = value;
		}
		else
		{
			ints.insert({ name, value });
		}
	}

	void SetTextureID(int id, const std::string name)
	{
		std::map<std::string, int>::iterator it = textures.find(name);
		if (it != textures.end())
		{
			it->second = id;
		}
		else
		{
			textures.insert({ name, id });
		}
	}

	void Use()
	{
		shader->Use();

		shader->SetMat4("model", *model);
		shader->SetMat4("view", *view);
		shader->SetMat4("projection", *projection);
		if (viewPosition != nullptr)
		{
			shader->SetVec3("viewPos", *viewPosition);
		}

		for (auto it = floats.begin(); it != floats.end(); it++)
		{
			shader->SetFloat(it->first, it->second);
		}

		for (auto it = vector3s.begin(); it != vector3s.end(); it++)
		{
			shader->SetVec3(it->first, it->second);
		}

		for (auto it = vector4s.begin(); it != vector4s.end(); it++)
		{
			shader->SetVec4(it->first, it->second);
		}

		for (auto it = ints.begin(); it != ints.end(); it++)
		{
			shader->SetInt(it->first, it->second);
		}

		for (auto it = textures.begin(); it != textures.end(); it++)
		{
			glActiveTexture(GL_TEXTURE0 + it->second);
			shader->SetInt(it->first, it->second);
			glBindTexture(GL_TEXTURE_2D, it->second);
		}
		glActiveTexture(GL_TEXTURE0);
	}

private:
	Shader* shader;

	glm::mat4* model = nullptr, * view = nullptr, * projection = nullptr;
	glm::vec3* viewPosition = nullptr;

	std::map<std::string, float> floats;
	std::map<std::string, int> ints;
	std::map<std::string, int> textures;
	std::map<std::string, glm::vec4> vector4s;
	std::map<std::string, glm::vec3> vector3s;
};
