#pragma once

#include "Mesh.h"
#include "Material.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Model
{
public:
	bool isPoint = false;

	// Constructor
	Model(const char* path, Material* mat, bool makePoint = false)
	{
		material = mat;
		isPoint = makePoint;
		if (path != nullptr)
		{
			LoadModel(path);
		}
		else if (makePoint)
		{
			meshes.push_back(Mesh(true));
		}
	}

	// Member functions
	void Draw()
	{
		for (size_t i = 0; i < meshes.size(); i++)
		{
			meshes[i].Draw(material);
		}
	}

	Material* GetMaterial() { return material; }
	void SetMaterial(Material* mat) { material = mat; }
	static GLuint TextureFromFile(const char* path, std::string directory);

private:
	// Model data
	std::vector<Mesh> meshes;
	std::string directory;
	std::vector<Texture> textures_loaded;

	Material* material;

	// Member functions
	void LoadModel(std::string path);
	void ProcessNode(aiNode* node, const aiScene* scene);
	Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};
