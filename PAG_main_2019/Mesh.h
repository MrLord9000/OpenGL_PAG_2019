#pragma once

#include "Material.h"
#include "glm/glm/glm.hpp"
#include <glad/glad.h>
#include <string>
#include <vector>
#include <exception>

// Vertex structure for storing basic vertex information
struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 tangent;
	glm::vec2 texCoords;
};

// Texture data structure
struct Texture
{
	GLuint id;
	std::string type;
	std::string path;
};

class Mesh
{
public:
	// Necessary mesh data -------
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<Texture> textures;

	// Constructor
	Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures)
	{
		this->vertices = vertices;
		this->indices = indices;
		this->textures = textures;

		SetupMesh();
	}

	Mesh(bool makePoint)
	{
		this->makePoint = makePoint;
		if (makePoint)
		{
			Vertex point;
			point.position = glm::vec3(0.0f, 0.0f, 0.0f);
			point.normal = glm::vec3(0.0f, 0.0f, 0.0f);
			point.texCoords = glm::vec2(0.0f, 0.0f);
			vertices.push_back(point);

			SetupMesh();
		}
		else
		{
			throw std::runtime_error("Made nothing as a mesh, this is a dirty hack xd.");
		}
	}

	// Drawing
	void Draw(Material* mat);

private:
	// Necessary render data --------
	GLuint VAO, VBO, EBO;

	bool makePoint = false;

	// Private member functions -----
	void SetupMesh();
};