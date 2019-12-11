#pragma once

#include "Shader.h"

#include <vector>
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
		floats.push_back(value);
		float_names.push_back(name);
	}

	void SetVec4(glm::vec4 value, const std::string name)
	{
		vector4s.push_back(value);
		vec4_names.push_back(name);
	}

	void SetVec3(glm::vec3 value, const std::string name)
	{
		vector3s.push_back(value);
		vec3_names.push_back(name);
	}

	void SetInt(int value, const std::string name)
	{
		ints.push_back(value);
		int_names.push_back(name);
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

		for (size_t i = 0; i < floats.size(); i++)
		{
			shader->SetFloat(float_names[i], floats[i]);
		}

		for (size_t i = 0; i < vector3s.size(); i++)
		{
			shader->SetVec3(vec3_names[i], vector3s[i]);
		}

		for (size_t i = 0; i < vector4s.size(); i++)
		{
			shader->SetVec4(vec4_names[i], vector4s[i]);
		}

		for (size_t i = 0; i < ints.size(); i++)
		{
			shader->SetInt(int_names[i], ints[i]);
		}
	}

private:
	Shader* shader;

	glm::mat4* model = nullptr, * view = nullptr, * projection = nullptr;
	glm::vec3* viewPosition = nullptr;

	std::vector<float> floats;
	std::vector<std::string> float_names;

	std::vector<glm::vec4> vector4s;
	std::vector<std::string> vec4_names;

	std::vector<glm::vec3> vector3s;
	std::vector<std::string> vec3_names;

	std::vector<int> ints;
	std::vector<std::string> int_names;
};
