#include "Mesh.h"

void Mesh::Draw(Material* mat)
{
	if (!makePoint)
	{
		GLuint diffuseNr = 1;
		GLuint specularNr = 1;
		GLuint occlusionNr = 1;
		GLuint normalNr = 1;

		// Handle shader textures =========================
		for (size_t i = 0; i < textures.size(); i++)
		{
			// Activate proper texture unit before binding
			glActiveTexture(GL_TEXTURE0 + i);

			// Retrieve texture number
			std::string number;
			std::string name = textures[i].type;

			if (name == "texture_diffuse")
			{
				number = std::to_string(diffuseNr);
				diffuseNr++;
			}
			else if (name == "texture_normal")
			{
				number = std::to_string(normalNr);
				normalNr++;
			}
			else if (name == "texture_roughness")
			{
				number = std::to_string(specularNr);
				specularNr++;
			}
			else if (name == "texture_metallic")
			{
				number = std::to_string(specularNr);
				specularNr++;
			}
			else if (name == "texture_occlusion")
			{
				number = std::to_string(occlusionNr);
				occlusionNr++;
			}

			std::string fullName = "material." + name + number;

			mat->GetShader()->SetInt(fullName.c_str(), i);
			glBindTexture(GL_TEXTURE_2D, textures[i].id);
		}
		//glActiveTexture(GL_TEXTURE0);

		mat->Use();

		// Draw the mesh
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
	else
	{
		mat->Use();

		glBindVertexArray(VAO);
		glDrawArrays(GL_POINTS, 0, 1);
		glBindVertexArray(0);
	}
}

void Mesh::SetupMesh()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	if (!makePoint)
	{
		glGenBuffers(1, &EBO);
	}

	// Bind vao and let the magic happen
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

	if (!makePoint)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
	}

	// Set attribute pointers
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

	if (!makePoint)
	{
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));

		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
	}

	glBindVertexArray(0);
}
